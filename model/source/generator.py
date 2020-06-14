import torch.nn as nn
import torch.nn.functional as F
from helpers import conv, deconv

class ResidualBlock(nn.Module):
    """
    Defines a residual block.
    It adds an input x to a convolutional layer (applied to x) with the same size input and output.
    Residual blocks allow a model to learn an effective transformation from one domain to another.
    """
    def __init__(self, conv_dim):
        super(ResidualBlock, self).__init__()

        # conv_dim = number of inputs  
        self.conv_dim = conv_dim
        
        # Define two convolutional layers + batch normalization that will act as our residual function F(x).
        # Layers should have the same shape input as output. The kernel_size of 3 is suggested.
        self.conv1 = conv(conv_dim, conv_dim, 3, 1, 1, batch_norm=True)
        self.conv2 = conv(conv_dim, conv_dim, 3, 1, 1, batch_norm=True)
                
    def forward(self, x):
        # apply a ReLu activation the outputs of the first layer
        out1 = F.relu(self.conv1(x))
        # return a summed output, x + resnet_block(x)  
        return x + self.conv2(out1)



class CycleGenerator(nn.Module):
    
    def __init__(self, image_size, conv_dim=64, n_res_blocks=6):
        """
        Instantiates the CycleGAN generator for images of size image_size,
        the conv_dim outputs produced by the first convolutional layer, and
        the number of residual blocks specified in n_res_blocks.
        """
        super(CycleGenerator, self).__init__()

        # save the expected image size (may come in handy for inference)
        self.image_size = image_size

        # Save the initialization parameters
        self.conv_dim = conv_dim
        self.n_res_blocks = n_res_blocks

        # Define the encoder part of the generator
        self.conv1 = conv(in_channels=3, out_channels=conv_dim, kernel_size=4, stride=2, padding=1, batch_norm=True)
        self.conv2 = conv(conv_dim, conv_dim*2, 4, 2, 1)
        self.conv3 = conv(conv_dim*2, conv_dim*4, 4, 2, 1)

        # Define the resnet part of the generator
        resnet_layers = []
        for _ in range(n_res_blocks):
            resnet_layers.append(ResidualBlock(conv_dim*4))
            
        self.resnet = nn.Sequential(*resnet_layers)

        # Define the decoder part of the generator
        self.deconv1 = deconv(in_channels=conv_dim*4, out_channels=conv_dim*2, 
                              kernel_size=4, stride=2, padding=1, batch_norm=True)
        self.deconv2 = deconv(in_channels=conv_dim*2, out_channels=conv_dim, 
                              kernel_size=4, stride=2, padding=1, batch_norm=True)
        self.deconv3 = deconv(conv_dim, 3, 4, 2, 1, batch_norm=False)

    def forward(self, x):
        """Given an image x, returns a transformed image."""
        # define feedforward behavior, applying activations as necessary
        x = F.relu(self.conv1(x)) # (3,128,128) -> (64, 64, 64)
        x = F.relu(self.conv2(x)) # (64, 64, 64) -> (128, 32, 32)
        x = F.relu(self.conv3(x)) # (256, 16, 16)
        x = self.resnet(x) 
        x = F.relu(self.deconv1(x)) # (256, 16, 16) -> (128, 32, 32)
        x = F.relu(self.deconv2(x)) # (128, 32, 32) -> (64, 64, 64)
        x = F.tanh(self.deconv3(x)) # (64, 64, 64) -> (3, 128, 128)
        return x

    def get_init_params(self):
        return (self.image_size, self.conv_dim, self.n_res_blocks)