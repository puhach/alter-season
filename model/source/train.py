from dataloader import get_data_loader
from display import imshow
from cyclegan import create_model, real_mse_loss, fake_mse_loss, cycle_consistency_loss, identity_mapping_loss
#from preprocess import scale
from helpers import scale, print_models, save_samples
from checkpoint import save_checkpoint, load_checkpoint, export_script_module
import torch
import torch.optim as optim
import os
import argparse


def train(train_dataloader_X, train_dataloader_Y, 
        test_dataloader_X, test_dataloader_Y, 
        device, n_epochs, balance,
        reconstruction_weight, identity_weight,
        print_every=1, checkpoint_every=10, sample_every=10):
    
    
    # keep track of losses over time
    losses = []

    #test_iter_X = iter(test_dataloader_X)
    #test_iter_Y = iter(test_dataloader_Y)

    # Get some fixed data from domains X and Y for sampling. These are images that are held
    # constant throughout training, that allow us to inspect the model's performance.
    fixed_X = next(iter(test_dataloader_X))[0] #test_iter_X.next()[0]
    fixed_Y = next(iter(test_dataloader_Y))[0] #test_iter_Y.next()[0]
    # make sure to scale to a range -1 to 1
    #fixed_X = scale(fixed_X) 
    #fixed_Y = scale(fixed_Y)
    fixed_X = scale(fixed_X.to(device))
    fixed_Y = scale(fixed_Y.to(device))

    # batches per epoch
    iter_X = iter(train_dataloader_X)
    iter_Y = iter(train_dataloader_Y)
    batches_per_epoch = min(len(iter_X), len(iter_Y))
    #tmp = min(len(train_dataloader_X), len(train_dataloader_Y))

    #bce_loss_crit = torch.nn.BCEWithLogitsLoss(reduction='mean')

    for epoch in range(1, n_epochs+1):

        epoch_loss_d_x = 0
        epoch_loss_d_y = 0
        epoch_loss_g = 0

        for _ in range(batches_per_epoch):

            ## Reset iterators for each epoch
            #if epoch % batches_per_epoch == 0:
            #    iter_X = iter(train_dataloader_X)
            #    iter_Y = iter(train_dataloader_Y)

            # move images to GPU or CPU depending on what is passed in the device parameter
            # make sure to scale to a range -1 to 1
            images_X, _ = next(iter_X)
            images_X = scale(images_X.to(device))
            #images_X, _ = iter_X.next()
            #images_X = scale(images_X) 

            #images_Y, _ = iter_Y.next()
            images_Y, _ = next(iter_Y)
            #images_Y = scale(images_Y)        
            images_Y = scale(images_Y.to(device))

            # move images to GPU or CPU depending on what is passed in the device parameter
            #images_X = images_X.to(device)
            #images_Y = images_Y.to(device)


            # ============================================
            #            TRAIN THE DISCRIMINATORS
            # ============================================
            
            ##   First: D_X, real and fake loss components   ##

            # Compute the discriminator losses on real images
            d_x_out = D_X(images_X)
            d_x_loss_real = real_mse_loss(d_x_out)
            #d_x_loss_real = bce_loss_crit(d_x_out, torch.ones_like(d_x_out))
            #d_x_loss_real = torch.nn.functional.binary_cross_entropy_with_logits(d_x_out, torch.ones_like(d_x_out))
            
            # Generate fake images that look like domain X based on real images in domain Y
            fake_x = G_YtoX(images_Y)

            # Compute the fake loss for D_X
            d_x_out = D_X(fake_x)
            d_x_loss_fake = fake_mse_loss(d_x_out)
            #d_x_loss_fake = bce_loss_crit(d_x_out, torch.zeros_like(d_x_out))
            #d_x_loss_fake = torch.nn.functional.binary_cross_entropy_with_logits(d_x_out, torch.zeros_like(d_x_out))
            
            # Compute the total loss
            d_x_loss = d_x_loss_real + d_x_loss_fake
            

            
            ##   Second: D_Y, real and fake loss components   ##
            
            d_y_out = D_Y(images_Y) 
            d_y_real_loss = real_mse_loss(d_y_out)  # D_y disciminator loss on a real Y image
            #d_y_real_loss = bce_loss_crit(d_y_out, torch.ones_like(d_y_out))
            #d_y_real_loss = torch.nn.functional.binary_cross_entropy_with_logits(d_y_out, torch.ones_like(d_y_out))
            
            fake_y = G_XtoY(images_X) # generate fake Y image from the real X image
            d_y_out = D_Y(fake_y)
            d_y_fake_loss = fake_mse_loss(d_y_out) # compute D_y loss on a fake Y image
            #d_y_fake_loss = bce_loss_crit(d_y_out, torch.zeros_like(d_y_out))
            #d_y_fake_loss = torch.nn.functional.binary_cross_entropy_with_logits(d_y_out, torch.zeros_like(d_y_out))
            
            d_y_loss = d_y_real_loss + d_y_fake_loss
            

            d_total_loss = d_x_loss + d_y_loss


            # =========================================
            #            TRAIN THE GENERATORS
            # =========================================

            ##    First: generate fake X images and reconstructed Y images    ##

            # Generate fake images that look like domain X based on real images in domain Y
            fake_x = G_YtoX(images_Y)

            # Compute the generator loss based on domain X
            d_out = D_X(fake_x)
            g_x_loss = real_mse_loss(d_out) # fake X should trick the D_x
            # TODO: consider using MSELoss or SmoothL1Loss (Huber loss)

            # Create a reconstructed y
            y_hat = G_XtoY(fake_x)
                    
            # Compute the cycle consistency loss (the reconstruction loss)
            rec_y_loss = cycle_consistency_loss(images_Y, y_hat, lambda_weight=reconstruction_weight)

            # Conversion from X to X should be an identity mapping
            it_x = G_YtoX(images_X)

            # Compute the identity mapping loss
            it_x_loss = identity_mapping_loss(images_X, it_x, weight=identity_weight)


            ##    Second: generate fake Y images and reconstructed X images    ##
            fake_y = G_XtoY(images_X)
            
            d_out = D_Y(fake_y)
            g_y_loss = real_mse_loss(d_out)  # fake Y should trick the D_y
            
            x_hat = G_YtoX(fake_y)
            
            rec_x_loss = cycle_consistency_loss(images_X, x_hat, lambda_weight=reconstruction_weight)

            it_y = G_XtoY(images_Y)

            it_y_loss = identity_mapping_loss(images_Y, it_y, weight=identity_weight)


            # Add up all generator and reconstructed losses 
            g_total_loss = g_x_loss + g_y_loss + rec_x_loss + rec_y_loss + it_x_loss + it_y_loss
            

            # Perform backprop
            
            if d_total_loss >= balance*g_total_loss:
                d_x_optimizer.zero_grad()
                d_x_loss.backward()
                d_x_optimizer.step()

                d_y_optimizer.zero_grad()
                d_y_loss.backward()
                d_y_optimizer.step()
            
            if g_total_loss >= balance*d_total_loss:
                g_optimizer.zero_grad()
                g_total_loss.backward()
                g_optimizer.step()

            # Gather statistics
            epoch_loss_d_x += d_x_loss.item()
            epoch_loss_d_y += d_y_loss.item()
            epoch_loss_g += g_total_loss.item()

            

        # Reset the iterators when epoch ends
        iter_X = iter(train_dataloader_X)
        iter_Y = iter(train_dataloader_Y)

        # Print the log info
        if epoch % print_every == 0 or epoch == n_epochs:
            # append real and fake discriminator losses and the generator loss
            losses.append((epoch_loss_d_x, epoch_loss_d_y, epoch_loss_g))
            print('Epoch [{:5d}/{:5d}] | d_X_loss: {:6.4f} | d_Y_loss: {:6.4f} | g_total_loss: {:6.4f}'.format(
                    epoch, n_epochs, epoch_loss_d_x, epoch_loss_d_y, epoch_loss_g))
            #losses.append((d_x_loss.item(), d_y_loss.item(), g_total_loss.item()))
            #print('Epoch [{:5d}/{:5d}] | d_X_loss: {:6.4f} | d_Y_loss: {:6.4f} | g_total_loss: {:6.4f}'.format(
            #        epoch, n_epochs, d_x_loss.item(), d_y_loss.item(), g_total_loss.item()))

            
        # Save the generated samples
        if epoch % sample_every == 0 or epoch == n_epochs:
            G_YtoX.eval() # set generators to eval mode for sample generation
            G_XtoY.eval()
            #save_samples(epoch, fixed_Y, fixed_X, G_YtoX, G_XtoY, device=device, batch_size=16, sample_dir='../samples')
            save_samples(epoch, fixed_Y, fixed_X, G_YtoX, G_XtoY, sample_dir='../samples')
            G_YtoX.train()
            G_XtoY.train()

        
        # Save the model parameters
        if epoch % checkpoint_every == 0 or epoch == n_epochs:
            save_checkpoint(G_XtoY, G_YtoX, D_X, D_Y, '../checkpoints')
            #export_script_modules(G_XtoY, G_YtoX, epoch, '../artifacts')
            export_script_module(G_XtoY, '../artifacts', 'summer_to_winter_{:05d}.sm'.format(epoch))   
            export_script_module(G_YtoX, '../artifacts', 'winter_to_summer_{:05d}.sm'.format(epoch))             

    return losses



parser = argparse.ArgumentParser(description='Alter season CycleGAN training script')
parser.add_argument('--device', type=str, default='cpu',
                    help='The device to use for training. Defaults to CPU.')
parser.add_argument('--epochs', type=int, required=True, 
                    help='The number of epochs to train for.')  
parser.add_argument('--batch', type=int, default=16, help='The batch size. Default is 16.')
parser.add_argument('--imsize', type=int, default=128, help='The size of input images. Defaults to 128.')
parser.add_argument('--cpt', type=int, default=10, 
                    help='The checkpointing frequency. By default a checkpoint is saved every 10 epochs.')
parser.add_argument('--sample', type=int, default=10, 
                    help='The sampling frequency. By default sample images are generated every 10 epochs.')
parser.add_argument('--lr', type=float, default=0.0001, help='The learning rate. Default is 0.0001.')  
parser.add_argument('--beta1', type=float, default=0.5, help='Beta1 parameter for the Adam optimizer.')  
parser.add_argument('--beta2', type=float, default=0.999, help='Beta2 parameter for the Adam optimizer.')                
parser.add_argument('--rw', type=float, default=10, help='Reconstruction loss weight. Default is 10.')
parser.add_argument('--iw', type=float, default=1, help='Identity mapping loss weight. Default is 1.')
parser.add_argument('--balance', type=float, default=0, help='If the ratio of the training loss to '
                    'the loss of an adversary becomes less than the balance value, training of the '
                    'superior network will stop. Default is 0, which means discriminators and generators '
                    'are unconstrained in their training progress.')


# Read command line arguments

args = parser.parse_args()

device = args.device
epochs = args.epochs
batch_size = args.batch
image_size = args.imsize
checkpoint_every = args.cpt
sample_every = args.sample

# hyperparameters for Adam optimizers
lr = args.lr
beta1=args.beta1
beta2=args.beta2
reconstruction_weight = args.rw
identity_weight = args.iw

balance = args.balance

print(f'Using {device} for training')

# Create train and test dataloaders for images from the two domains X and Y

trainloader_X = get_data_loader(image_type='summer', image_dir='../data/train', shuffle=True, image_size=image_size, batch_size=batch_size)
trainloader_Y = get_data_loader(image_type='winter', image_dir='../data/train', shuffle=True, image_size=image_size, batch_size=batch_size)

# TODO: remove test loaders later
testloader_X = get_data_loader(image_type='summer', image_dir='../data/test', shuffle=False, image_size=image_size, batch_size=batch_size)
testloader_Y = get_data_loader(image_type='winter', image_dir='../data/test', shuffle=False, image_size=image_size, batch_size=batch_size)


#device = 'cuda' if torch.cuda.is_available() else 'cpu'

# instantiate the complete model
G_XtoY, G_YtoX, D_X, D_Y = create_model(image_size=image_size, g_conv_dim=64, d_conv_dim=64, n_res_blocks=6, device=device)

## print the model architecture
#print_models(G_XtoY, G_YtoX, D_X, D_Y)



g_params = list(G_XtoY.parameters()) + list(G_YtoX.parameters())  # Get generator parameters

# Create optimizers for the generators and discriminators
g_optimizer = optim.Adam(g_params, lr, [beta1, beta2])
d_x_optimizer = optim.Adam(D_X.parameters(), lr, [beta1, beta2])
d_y_optimizer = optim.Adam(D_Y.parameters(), lr, [beta1, beta2])



losses = train(trainloader_X, trainloader_Y, testloader_X, testloader_Y, 
                device=device, n_epochs=epochs, balance=balance,
                reconstruction_weight=reconstruction_weight,
                identity_weight=identity_weight,
                checkpoint_every=checkpoint_every, 
                sample_every=sample_every)


## Load the checkpoint
#G_XtoY, G_YtoX, D_X, D_Y = load_checkpoint('../checkpoints', device='cpu')
#
## Export the generators
#print('Creating script modules...')
#artifact_dir = '../artifact'
#os.makedirs(artifact_dir, exist_ok=True)
#sm_g_x_to_y = torch.jit.script(G_XtoY)
#sm_g_x_to_y.save(os.path.join(artifact_dir, 'summer_to_winter.sm'))
#sm_g_y_to_x = torch.jit.script(G_YtoX)
#sm_g_y_to_x.save(os.path.join(artifact_dir, 'winter_to_summer.sm'))
#
#
## Test the script modules
#print('Testing the script modules...')
#
#batch = next(iter(testloader_X))
#scaled_img = scale(batch[0][0])
#test_y = sm_g_x_to_y(scaled_img.unsqueeze(0))
#print('x -> y:', test_y.shape)
#
#batch = next(iter(testloader_Y))
#scaled_img = scale(batch[0][0])
#test_x = sm_g_y_to_x(scaled_img.unsqueeze(0))
#print('y -> x:', test_x.shape)

print('Done!')
