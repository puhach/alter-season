# Alter Season Model

This project creates a generative model based on a CycleGAN architecture. The model should read an image from a summer set and transform it so that it looks as if it belongs to a winter set (or vice versa). A CycleGAN is designed for image-to-image translation and it learns from unpaired training data. This means that in order to train a generator to translate images from one domain to another, we don't need to have exact correspondences between individual images in those domains. For example, in the paper that introduced CycleGANs, the authors were able to translate between images of horses and zebras, even though there are no images of a zebra in exactly the same position as a horse or with exactly the same background, etc. To train the model we'll be using a set of images of Yosemite national park taken either during the summer of winter. The summer and winter seasons are the two domains for image-to-image translation.

![summer-to-winter-cover](./assets/cover.jpg)


## Project Set Up

Download and install [Miniconda](https://docs.conda.io/en/latest/miniconda.html) with Python 3.

Create a new conda environment:
```
conda create -n alter-season python=3.7
```

Install PyTorch:
```
conda install pytorch=1.4 torchvision cudatoolkit=10.1 -c pytorch
```

Install imageio:
```
pip install imageio
```

Clode the alter-season repo:
```
git clone https://github.com/puhach/alter-season.git
```

Download the [dataset](https://video.udacity-data.com/topher/2018/November/5be66e78_summer2winter-yosemite/summer2winter-yosemite.zip).


If you like to train the model in a cloud, the training data can be uploaded to a cloud instance (e.g. EC2) by this command:
```
scp -i /directory/to/your-ssh-key.pem /your/local/file/to/copy user@ec2-xx-xx-xxx-xxx.compute-1.amazonaws.com:path/to/file 
```

Extract the training data to `model/data`.

The training script expects a folder structure which is different from the original dataset. Adjust it according to the tree below:
```
data
├───test
│   ├───summer
│   │   └───summer
│   └───winter
│       └───winter
└───train
    ├───summer
    │   └───summer
    └───winter
        └───winter
```

## Training

Run training from the `model/source` directory, e.g.:
```
python train.py --epochs 100 --device cuda 
```

The following table lists the command line arguments supported by the training script:
| Parameter | Explanation |
|-----------|-------------|
|  -h, --help |        show this help message and exit |
|  --device DEVICE |   The device to use for training. Defaults to CPU. |
|  --epochs EPOCHS |   The number of epochs to train for. |
|  --batch BATCH |     The batch size. Default is 16. |
|  --imsize IMSIZE |   The size of input images. Defaults to 128. |
|  --cpt CPT |         The checkpointing frequency. By default a checkpoint is saved every 10 epochs. |
|  --sample SAMPLE |   The sampling frequency. By default sample images are  generated every 10 epochs. |
|  --lr LR |           The learning rate. Default is 0.0001. |
|  --beta1 BETA1 |     Beta1 parameter for the Adam optimizer. |
|  --beta2 BETA2 |     Beta2 parameter for the Adam optimizer. |
|  --rw RW |           Reconstruction loss weight. Default is 10. |
|  --iw IW |           Identity mapping loss weight. Default is 1. |
|  --balance BALANCE | If the ratio of the training loss to the loss of an  adversary becomes less than the balance value, training of the superior network will stop. Default is 0, which means discriminators and generators are unconstrained in their training progress. |


## Credits

The project is based on the [CycleGAN lesson](https://classroom.udacity.com/nanodegrees/nd101/parts/2ea78ff8-befd-4046-b06e-5327871b0748/modules/ae797d2c-df0b-4aed-b586-955e8034c580/lessons/f5f47d94-6ed5-4adb-bf25-6d82601b0b8f/concepts/95118d2e-3129-42e8-8808-0ffacd907779) from the [Deep Learning Nanodegree](https://www.udacity.com/course/deep-learning-nanodegree--nd101) at Udacity and the original [CycleGAN paper](https://arxiv.org/abs/1703.10593).