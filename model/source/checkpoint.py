import os
import torch
from discriminator import Discriminator
from generator import CycleGenerator



# TODO: move save/load functions to checkpoint.py

def save_instance(instance, file_path):
    """
    Saves the initialization parameters and the state dictionary of the discriminator or the generator.
    """
    torch.save({
        'init_params': instance.get_init_params(),
        'state_dict': instance.state_dict()
    }, file_path)    

#def checkpoint(iteration, G_XtoY, G_YtoX, D_X, D_Y, checkpoint_dir='checkpoints_cyclegan'):
#def save_checkpoint(epoch, G_XtoY, G_YtoX, D_X, D_Y, checkpoint_dir):
def save_checkpoint(G_XtoY, G_YtoX, D_X, D_Y, checkpoint_dir):
    """
    Saves the parameters of all generators and discriminators.
    """

    #epoch_dir = f'{epoch: <{4}}'
    #os.makedirs(os.path.join(checkpoint_dir, epoch_dir), exist_ok=True)
    os.makedirs(checkpoint_dir, exist_ok=True)
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
