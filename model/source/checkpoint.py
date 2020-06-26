import os
import torch
import copy
from discriminator import Discriminator
from generator import CycleGenerator



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

def load_instance(instance_class, file_path, device):
    model_info = torch.load(file_path, map_location=device)
    init_params = model_info['init_params']
    instance = instance_class(*init_params)
    instance.load_state_dict(model_info['state_dict'])
    instance.eval()
    instance = instance.to(device)
    return instance

def load_checkpoint(checkpoint_dir, device):
    """
    Load all discriminators and generators. 
    """
    G_XtoY = load_instance(CycleGenerator, os.path.join(checkpoint_dir, 'g_x_to_y.pt'), device)
    G_YtoX = load_instance(CycleGenerator, os.path.join(checkpoint_dir, 'g_y_to_x.pt'), device)
    D_X = load_instance(Discriminator, os.path.join(checkpoint_dir, 'd_x.pt'), device)
    D_Y = load_instance(Discriminator, os.path.join(checkpoint_dir, 'd_y.pt'), device)
    return G_XtoY, G_YtoX, D_X, D_Y


def export_script_module(G, export_dir, file_name):
    """
    Convert a PyTorch model to a TorchScript module, which can be loaded from a C++ program.
    """
    os.makedirs(export_dir, exist_ok=True)
    
    G_cpu = copy.deepcopy(G).cpu()
    #G_cpu = G.__class__(G.get_init_params())
    #G_cpu.load_state_dict(G.state_dict())

    sm = torch.jit.script(G_cpu)
    sm.save(os.path.join(export_dir, file_name))


#def export_script_modules(G_XtoY, G_YtoX, epoch, export_dir):
#    """
#    Convert the generators from PyTorch models to TorchScript modules, which can be loaded from a C++ program.
#    """
#
#    os.makedirs(export_dir, exist_ok=True)    
#    
#    #sm_g_x_to_y = torch.jit.script(G_XtoY.cpu())
#    G_XtoY_cpu = CycleGenerator(G_XtoY.get_init_params())
#    G_XtoY_cpu.load_state_dict(G_XtoY.state_dict())
#    sm_g_x_to_y = torch.jit.script(G_XtoY_cpu)
#
#    # sm_g_y_to_x = torch.jit.script(G_YtoX.cpu())
#    G_YtoX_cpu = CycleGenerator(G_YtoX.get_init_params())
#    G_YtoX_cpu.load_state_dict(G_YtoX.state_dict())
#    sm_g_y_to_x = torch.jit.script(G_YtoX_cpu)
#
#    sm_g_x_to_y.save(os.path.join(export_dir, 'summer_to_winter_e{:05d}.sm'.format(epoch)))
#    
#    sm_g_y_to_x.save(os.path.join(export_dir, 'winter_to_summer_e{:05d}.sm'.format(epoch)))


