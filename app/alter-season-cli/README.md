# Alter Season CLI

This project is a simple command line utility which uses a CycleGAN model developed with PyTorch for image conversion. 

Refer to [this](../../model/README.md) project in case you are interested in model implementation itself.


## Project Set Up

Download LibTorch. The version used for development is 1.4.

LibTorch 1.4 CPU (debug):
https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-debug-1.4.0.zip

LibTorch 1.4 CPU (release):
https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-1.4.0.zip

LibTorch 1.4 with CUDA 10.1 (debug):
https://download.pytorch.org/libtorch/cu101/libtorch-win-shared-with-deps-debug-1.4.0.zip

LibTorch 1.4 with CUDA 10.1 (release):
https://download.pytorch.org/libtorch/cu101/libtorch-win-shared-with-deps-1.4.0.zip


Extract LibTorch. 

In CMakeSettings.json set Torch_DIR variable to the path to extracted LibTorch library. 

When adding a release configuration, configurationType is set to "RelWithDebInfo" by default. It seems to be incompatible with release version of LibTorch. Change configurationType to "Release" or use the debug version of LibTorch. 



## Usage

The program takes in two command line arguments: the path to the script module and the input image. 

For example, to convert a summer picture summer2.jpg to winter by means of the summer_to_winter_torch14_256x256_00100.sm module, run this command:
```
alter-season-cli.exe path/to/module/summer_to_winter_torch14_256x256_00100.sm path/to/image/summer2.jpg 
```

## Credits


This project is based on the "Loading a TorchScript Model in C++" [tutorial](https://pytorch.org/tutorials/advanced/cpp_export.html).
