import torch
import torch.nn as nn
import torch.nn.functional as F
import os
import numpy as np


def scale(x, feature_range=(-1, 1)):
    """
    Scale takes in an image x and returns that image, scaled
    with a feature_range of pixel values from -1 to 1. 
    This function assumes that the input x is already scaled from 0-255.
    """
    
    # scale from 0-1 to feature_range
    min, max = feature_range
    x = x * (max - min) + min
    return x




def conv(in_channels, out_channels, kernel_size, stride=2, padding=1, batch_norm=True):
    """
    Creates a convolutional layer with optional batch normalization.
    """
    layers = []
    # TODO: ? Shouldn't bias be set to NOT batch_norm instead of always being False ? 
    conv_layer = nn.Conv2d(in_channels=in_channels, out_channels=out_channels, 
                           kernel_size=kernel_size, stride=stride, padding=padding, bias=False)
    
    layers.append(conv_layer)

    if batch_norm:
        layers.append(nn.BatchNorm2d(out_channels))
    return nn.Sequential(*layers)


# helper deconv function
def deconv(in_channels, out_channels, kernel_size, stride=2, padding=1, batch_norm=True):
    """
    Creates a transpose convolutional layer, with optional batch normalization.
    """
    layers = []
    # append transpose conv layer
    # ?? Shouldn't we set bias to NOT batch_norm instead of always being False ?
    layers.append(nn.ConvTranspose2d(in_channels, out_channels, kernel_size, stride, padding, bias=False))
    # optional batch norm layer
    if batch_norm:
        layers.append(nn.BatchNorm2d(out_channels))
    return nn.Sequential(*layers)


# helper function for printing the model architecture
def print_models(G_XtoY, G_YtoX, D_X, D_Y):
    """Prints model information for the generators and discriminators.
    """
    print("                     G_XtoY                    ")
    print("-----------------------------------------------")
    print(G_XtoY)
    print()

    print("                     G_YtoX                    ")
    print("-----------------------------------------------")
    print(G_YtoX)
    print()

    print("                      D_X                      ")
    print("-----------------------------------------------")
    print(D_X)
    print()

    print("                      D_Y                      ")
    print("-----------------------------------------------")
    print(D_Y)
    print()


def save_instance(instance, file_path):
    """
    Saves the initialization parameters and the state dictionary of the discriminator or the generator.
    """
    torch.save({
        'init_params': instance.get_init_params(),
        'state_dict': instance.state_dict()
    }, file_path)    

#def checkpoint(iteration, G_XtoY, G_YtoX, D_X, D_Y, checkpoint_dir='checkpoints_cyclegan'):
def save_checkpoint(epoch, G_XtoY, G_YtoX, D_X, D_Y, checkpoint_dir):
    """
    Saves the parameters of all generators and discriminators.
    """

    epoch_dir = f'{epoch: <{4}}'
    os.makedirs(os.path.join(checkpoint_dir, epoch_dir), exist_ok=True)
    #checkpoint_dir = os.path.dirname(filepath)
    #if checkpoint_dir != '':
    #    os.makedirs(checkpoint_dir, exist_ok=True)

    save_instance(G_XtoY, os.path.join(checkpoint_dir, 'g_x_to_y.pt'))
    save_instance(G_YtoX, os.path.join(checkpoint_dir, 'g_y_to_x.pt'))
    save_instance(D_X, os.path.join(checkpoint_dir, 'd_x.pt'))
    save_instance(D_Y, os.path.join(checkpoint_dir, 'd_y.pt'))


    #obj_dict = { 
    #    'g_summer_to_winter': G_XtoY,
    #    'g_winter_to_summer': G_YtoX,
    #    'd_summer': D_X,
    #    'd_winter': D_Y 
    #}
    #
    #for name, obj in obj_dict.items():
    #    file_path = os.path.join(checkpoint_dir, name + '.pt')
    #    torch.save({
    #        'init_params': obj.get_init_params(),
    #        'state_dict': obj.state_dict()
    #    }, file_path)
        



    #torch.save(
    #    {
    #        'g_x_to_y_init_params': G_XtoY.get_init_params(),
    #        'g_x_to_y_state_dict': G_XtoY.state_dict(),
    #        'g_y_to_x_init_params': G_YtoX.get_init_params(),
    #        'g_y_to_x_state_dict': G_YtoX.state_dict(),
    #        'd_x_init_params': D_X.get_init_params(),
    #        'd_x_state_dict': D_X.state_dict(),
    #        'd_y_init_params': D_Y.get_init_params(),
    #        'd_y_state_dict': D_Y.state_dict()
    #    }, filepath)

    #G_XtoY_path = os.path.join(checkpoint_dir, 'G_XtoY.pkl')
    #G_YtoX_path = os.path.join(checkpoint_dir, 'G_YtoX.pkl')
    #D_X_path = os.path.join(checkpoint_dir, 'D_X.pkl')
    #D_Y_path = os.path.join(checkpoint_dir, 'D_Y.pkl')
    #torch.save(G_XtoY.state_dict(), G_XtoY_path)
    #torch.save(G_YtoX.state_dict(), G_YtoX_path)
    #torch.save(D_X.state_dict(), D_X_path)
    #torch.save(D_Y.state_dict(), D_Y_path)





def merge_images(sources, targets, batch_size=16):
    """
    Creates a grid consisting of pairs of columns, where the first column in
    each pair contains images source images and the second column in each pair
    contains images generated by the CycleGAN from the corresponding images in
    the first column.
    """
    _, _, h, w = sources.shape
    row = int(np.sqrt(batch_size))
    merged = np.zeros([3, row*h, row*w*2])
    for idx, (s, t) in enumerate(zip(sources, targets)):
        i = idx // row
        j = idx % row
        merged[:, i*h:(i+1)*h, (j*2)*h:(j*2+1)*h] = s
        merged[:, i*h:(i+1)*h, (j*2+1)*h:(j*2+2)*h] = t
    merged = merged.transpose(1, 2, 0)
    return merged
    

def to_data(x):
    """
    Converts a tensor to numpy array.
    """

    if torch.cuda.is_available():
        x = x.cpu()
    x = x.data.numpy()
    x = ((x +1)*255 / (2)).astype(np.uint8) # rescale to 0-255
    return x


def save_samples(iteration, fixed_Y, fixed_X, G_YtoX, G_XtoY, batch_size=16, sample_dir='samples_cyclegan'):
    """
    Saves samples from both generators X->Y and Y->X.
    """

    # move input data to correct device
    device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

    fake_X = G_YtoX(fixed_Y.to(device))
    fake_Y = G_XtoY(fixed_X.to(device))
    
    X, fake_X = to_data(fixed_X), to_data(fake_X)
    Y, fake_Y = to_data(fixed_Y), to_data(fake_Y)
    
    merged = merge_images(X, fake_Y, batch_size)
    path = os.path.join(sample_dir, 'sample-{:06d}-X-Y.png'.format(iteration))
    scipy.misc.imsave(path, merged)
    print('Saved {}'.format(path))
    
    merged = merge_images(Y, fake_X, batch_size)
    path = os.path.join(sample_dir, 'sample-{:06d}-Y-X.png'.format(iteration))
    scipy.misc.imsave(path, merged)
    print('Saved {}'.format(path))
