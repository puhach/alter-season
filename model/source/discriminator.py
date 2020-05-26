import torch.nn as nn
import torch.nn.functional as F
from helpers import conv

class Discriminator(nn.Module):
    
    def __init__(self, conv_dim=64):
        super(Discriminator, self).__init__()

        # Save the initialization parameters
        self.conv_dim = conv_dim

        # Define all convolutional layers
        self.conv1 = conv(in_channels=3, out_channels=conv_dim, 
                          kernel_size=4, stride=2, padding=1, batch_norm=False)
        
        self.conv2 = conv(conv_dim, conv_dim*2, 4, 2, 1)
        self.conv3 = conv(conv_dim*2, conv_dim*4, 4, 2, 1)
        self.conv4 = conv(conv_dim*4, conv_dim*8, 4, 2, 1)
        self.conv5 = nn.Conv2d(conv_dim*8, 1, kernel_size=8, padding=0)
        
        # Should accept an RGB image as input and output a single value
        

    def forward(self, x):
        # define feedforward behavior
        #print("x:", x.shape)
        x = F.relu(self.conv1(x)) # (3, 128, 128) -> (64, 64, 64)
        x = F.relu(self.conv2(x)) # (64, 64, 64) -> (128, 32, 32)
        x = F.relu(self.conv3(x)) # (128, 32, 32) -> (256, 16, 16)
        x = F.relu(self.conv4(x)) # (256, 16, 16) -> (512, 8, 8)
        #x = self.conv5(x) # (512,8,8) -> (1, 1, 1)
        x = F.sigmoid(self.conv5(x))
        return x

    def get_init_params(self):
        return (self.conv_dim, )