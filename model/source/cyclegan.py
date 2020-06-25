from discriminator import Discriminator
from generator import CycleGenerator
import torch


def create_model(image_size, g_conv_dim=64, d_conv_dim=64, n_res_blocks=6, device='cpu'):
    """Builds the generators and discriminators."""
    
    # Instantiate generators
    G_XtoY = CycleGenerator(image_size, g_conv_dim, n_res_blocks)
    G_YtoX = CycleGenerator(image_size, g_conv_dim, n_res_blocks)
    
    # Instantiate discriminators
    D_X = Discriminator(d_conv_dim)
    D_Y = Discriminator(d_conv_dim)

    # move models to GPU, if specified
    G_XtoY.to(device)
    G_YtoX.to(device)
    D_X.to(device)
    D_Y.to(device)

    # move models to GPU, if available
    #if torch.cuda.is_available():
    #    device = torch.device("cuda:0")
    #    G_XtoY.to(device)
    #    G_YtoX.to(device)
    #    D_X.to(device)
    #    D_Y.to(device)
    #    print('Models moved to GPU.')
    #else:
    #    print('Only CPU available.')

    return G_XtoY, G_YtoX, D_X, D_Y


def real_mse_loss(D_out):
    # how close is the produced output to being "real"?
    return torch.mean((D_out - 1)**2)

def fake_mse_loss(D_out):
    # how close is the produced output to being "false"?
    return torch.mean((D_out - 0)**2)

def cycle_consistency_loss(real_im, reconstructed_im, lambda_weight):
    # calculate the reconstruction loss 
    loss = torch.mean(torch.abs(real_im - reconstructed_im))
    # return weighted loss
    return lambda_weight*loss

def identity_mapping_loss(real_im, generated_im, weight):
    # calculate the identity mapping loss
    loss = torch.nn.functional.l1_loss(real_im, generated_im, reduction='mean')
    # return weighted loss
    return weight*loss