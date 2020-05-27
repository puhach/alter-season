import torch
import torch.nn as nn
import torch.nn.functional as F
import os
import numpy as np
import imageio
import math



def scale(x, feature_range=(-1, 1)):
    """
    Scale takes in an image x and returns that image, scaled
    with a feature_range of pixel values from -1 to 1. 
    This function assumes that the input x is already scaled from 0-255.
    """
    
    # scale from 0-1 to feature_range
    min, max = feature_range
    #x = x * (max - min) + min
    x = torch.add(torch.mul(x, (max-min)), min)
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





#def merge_images(sources, targets, batch_size=16):
def merge_images(sources, targets):
    """
    Creates a grid consisting of pairs of columns, where the first column in
    each pair contains images source images and the second column in each pair
    contains images generated by the CycleGAN from the corresponding images in
    the first column.
    """
    batch_size, channels, h, w = sources.shape
    rows = int(np.sqrt(batch_size))
    cols = int(math.ceil(batch_size / rows))
    #merged = np.zeros([3, row*h, row*w*2])
    grid = np.zeros([channels, rows*h, cols*w*2], dtype=np.uint8)
    for idx, (s, t) in enumerate(zip(sources, targets)):
        #i = idx // row
        #j = idx % row
        i = idx // rows
        j = idx % rows
        grid[:, i*h:(i+1)*h, (j*2)*h:(j*2+1)*h] = s
        grid[:, i*h:(i+1)*h, (j*2+1)*h:(j*2+2)*h] = t
    grid = grid.transpose(1, 2, 0)
    return grid
    

def to_data(x):
    """
    Converts a tensor to numpy array.
    """

    # TODO: try to scale back in pytorch, then convert to numpy
    if torch.cuda.is_available():
        x = x.cpu()
    x = x.data.numpy()
    x = ((x + 1)*255 / (2)).astype(np.uint8) # rescale to 0-255
    return x


def save_samples(iteration, fixed_Y, fixed_X, G_YtoX, G_XtoY, 
                device, batch_size=16, sample_dir='samples'):
    """
    Saves samples from both generators X->Y and Y->X.
    """

    # TODO: don't move here, pass the moved tensor instead
    # move input data to the correct device
    fake_X = G_YtoX(fixed_Y.to(device))
    fake_Y = G_XtoY(fixed_X.to(device))
    
    X, fake_X = to_data(fixed_X), to_data(fake_X)
    Y, fake_Y = to_data(fixed_Y), to_data(fake_Y)
    
    #grid_xy = merge_images(X, fake_Y, batch_size)
    grid_xy = merge_images(X, fake_Y)
    path = os.path.join(sample_dir, 'sample-{:06d}-X-Y.png'.format(iteration))
    #scipy.misc.imsave(path, merged)
    imageio.imwrite(path, grid_xy)
    #print('Saved {}'.format(path))
    
    #grid_yx = merge_images(Y, fake_X, batch_size)
    grid_yx = merge_images(Y, fake_X)
    path = os.path.join(sample_dir, 'sample-{:06d}-Y-X.png'.format(iteration))
    #scipy.misc.imsave(path, merged)
    imageio.imwrite(path, grid_yx)
    #print('Saved {}'.format(path))
