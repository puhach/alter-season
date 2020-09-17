# Alter Season CLI

This project is a simple command line utility which uses a CycleGAN model developed with PyTorch for image conversion. 

Refer to [this](../../model/README.md) project in case you are interested in model implementation itself.


## Project Set Up

Download and install [Git extension for versioning large files](https://git-lfs.github.com/). On Linux with the packagecloud repository configured for your system, you can install Git LFS from the command line:  
```
sudo apt-get install git-lfs
```

Then set up Git LFS:
```
git lfs install
```

Once it is done, clone [this](https://github.com/puhach/alter-season.git) repository.

Download LibTorch. The version used for development is 1.4.

LibTorch 1.4 CPU for Windows (Debug):
https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-debug-1.4.0.zip

LibTorch 1.4 CPU for Windows (Release):
https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-1.4.0.zip

LibTorch 1.4 with CUDA 10.1 for Windows (Debug):
https://download.pytorch.org/libtorch/cu101/libtorch-win-shared-with-deps-debug-1.4.0.zip

LibTorch 1.4 with CUDA 10.1 for Windows (Release):
https://download.pytorch.org/libtorch/cu101/libtorch-win-shared-with-deps-1.4.0.zip

LibTorch 1.4 CPU for Linux (C++11 ABI):
https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.4.0%2Bcpu.zip

LibTorch 1.4 with CUDA 10.1 for Linux (C++11 ABI):
https://download.pytorch.org/libtorch/cu101/libtorch-cxx11-abi-shared-with-deps-1.4.0.zip



Extract LibTorch. 

Specify paths to LibTorch and OpenCV cmake files in Torch_DIR and OpenCV_DIR variables respectively. Then generate build files, e.g.:
```
mkdir build
cd build
cmake .. -DTorch_DIR=/opt/libtorch-1.4-cpu/share/cmake/Torch -DOpenCV_DIR=/opt/opencv/4.4.0/installation/lib/cmake/opencv4
```

According to PyTorch guidelines, the path has to be absolute.

Now everything is ready to build the project:
```
cmake --build . --config Release
```

When you are on Windows using Visual Studio, you can Torch_DIR and OpenCV_DIR in CMakeSettings.json. When adding a release configuration, configurationType is set to "RelWithDebInfo" by default. It seems to be incompatible with release version of LibTorch. Change configurationType to "Release" or use the debug version of LibTorch. 



## Usage

The program takes in two command line arguments: the path to the script module and the input image. 

For example, to convert a summer picture summer2.jpg to winter by means of the summer_to_winter_torch14_256x256_00100.sm module, run this command:
```
alter-season-cli.exe path/to/module/summer_to_winter_torch14_256x256_00100.sm path/to/image/summer2.jpg 
```

## Credits


This project is based on the "Loading a TorchScript Model in C++" [tutorial](https://pytorch.org/tutorials/advanced/cpp_export.html).
