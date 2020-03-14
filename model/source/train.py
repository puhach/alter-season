import os
import torch
from torch.utils.data import DataLoader
import torchvision
import torchvision.datasets as datasets
import torchvision.transforms as transforms


parser = argparse.ArgumentParser()
args = parser.parse_args()
print(args)