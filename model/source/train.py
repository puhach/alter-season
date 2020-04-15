from dataloader import get_data_loader



# Create train and test dataloaders for images from the two domains X and Y
# image_type = directory names for our data
trainloader_X = get_data_loader(image_type='summer', image_dir='../data/train', shuffle=True)
trainloader_Y = get_data_loader(image_type='winter', image_dir='../data/train', shuffle=True)

batch = next(iter(trainloader_X))
print(batch)
#parser = argparse.ArgumentParser()
#args = parser.parse_args()
#print(args)