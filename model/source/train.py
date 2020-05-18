from dataloader import get_data_loader
from display import imshow
from cyclegan import create_model, real_mse_loss, fake_mse_loss, cycle_consistency_loss
#from preprocess import scale
from helpers import scale, print_models
import torch.optim as optim


# Create train and test dataloaders for images from the two domains X and Y
# image_type = directory names for our data
trainloader_X = get_data_loader(image_type='summer', image_dir='../data/train', shuffle=True)
trainloader_Y = get_data_loader(image_type='winter', image_dir='../data/train', shuffle=True)

batch = next(iter(trainloader_X))
print(batch)

img = batch[0][0]

print('Min: ', img.min())
print('Max: ', img.max())

scaled_img = scale(img)

print('Scaled min: ', scaled_img.min())
print('Scaled max: ', scaled_img.max())

# instantiate the complete model
G_XtoY, G_YtoX, D_X, D_Y = create_model()

# print the model architecture
print_models(G_XtoY, G_YtoX, D_X, D_Y)




# hyperparams for Adam optimizers
lr=0.0002
beta1=0.5
beta2=0.999 

g_params = list(G_XtoY.parameters()) + list(G_YtoX.parameters())  # Get generator parameters

# Create optimizers for the generators and discriminators
g_optimizer = optim.Adam(g_params, lr, [beta1, beta2])
d_x_optimizer = optim.Adam(D_X.parameters(), lr, [beta1, beta2])
d_y_optimizer = optim.Adam(D_Y.parameters(), lr, [beta1, beta2])