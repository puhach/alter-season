from dataloader import get_data_loader
from display import imshow
from cyclegan import create_model, real_mse_loss
#from preprocess import scale
from helpers import scale, print_models

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