import torch.nn as nn
import torch.nn.functional as F
from helpers import conv

class ResidualBlock(nn.Module):
    """Defines a residual block.
       This adds an input x to a convolutional layer (applied to x) with the same size input and output.
       These blocks allow a model to learn an effective transformation from one domain to another.
    """
    def __init__(self, conv_dim):
        super(ResidualBlock, self).__init__()
        # conv_dim = number of inputs  
        self.conv_dim = conv_dim
        
        # define two convolutional layers + batch normalization that will act as our residual function, F(x)
        # layers should have the same shape input as output; I suggest a kernel_size of 3
        self.conv1 = conv(conv_dim, conv_dim, 3, 1, 1, batch_norm=True)
        self.conv2 = conv(conv_dim, conv_dim, 3, 1, 1, batch_norm=True)
                
    def forward(self, x):
        # apply a ReLu activation the outputs of the first layer
        out1 = F.relu(self.conv1(x))
        # return a summed output, x + resnet_block(x)  
        return x + self.conv2(out1)