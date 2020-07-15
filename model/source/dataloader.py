import os
import torch
from torch.utils.data import DataLoader
import torchvision
import torchvision.datasets as datasets
import torchvision.transforms as transforms



def get_data_loader(image_type, image_dir, shuffle=True,
                    image_size=128, batch_size=16, num_workers=0):
    """
    Returns training and test data loaders for a given image type, either 'summer' or 'winter'. 
    These images will be resized to image_size x image_size x 3 and converted into Tensors.
    """
    
    # resize the images
    # TODO: consider cropping to make images square 
    transform = transforms.Compose([transforms.Resize(image_size), 
                                    transforms.ToTensor()])

    # get training and test directories
    image_path = os.path.join(image_dir, image_type)

    # define datasets using ImageFolder
    train_dataset = datasets.ImageFolder(image_path, transform)

    # create and return DataLoaders
    train_loader = DataLoader(dataset=train_dataset, batch_size=batch_size, shuffle=shuffle, num_workers=num_workers)

    return train_loader



