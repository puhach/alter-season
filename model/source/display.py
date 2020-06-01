#import matplotlib.pyplot as plt
import numpy as np


def imshow(img):
    npimg = img.numpy()
    plt.imshow(np.transpose(npimg, (1, 2, 0)))


# show images
#fig = plt.figure(figsize=(12, 8))
#imshow(torchvision.utils.make_grid(images))