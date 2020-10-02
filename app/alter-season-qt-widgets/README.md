# Alter Season GUI

![cover](./assets/cover.png)

This project implements a Qt GUI application which uses a CycleGAN model for image conversion. 

Refer to [this](../../model/README.md) project in case you are interested in model implementation itself.


## Project Set Up

For find_package(Qt5) to be successful, set your CMAKE_PREFIX_PATH environment variable to the Qt 5 installation prefix, e.g.:
```
cmake.exe .. -DCMAKE_PREFIX_PATH="C:\Qt\5.14.2\5.14.2\msvc2017_64" 
```

In order to help cmake find Torch, set Torch_DIR to the path containing TorchConfig.cmake, e.g.:
```
set(Torch_DIR "D:/dox/projects/AI/alter-season-cyclegan/app/common/libtorch-1.4-cpu-release/share/cmake/Torch")
```

Alternatively, extend CMAKE_PREFIX_PATH to include the path to LibTorch:
```
cmake.exe .. -DCMAKE_PREFIX_PATH="C:\Qt\5.14.2\5.14.2\msvc2017_64;D:/dox/projects/AI/alter-season-cyclegan/app/common/libtorch-1.4-cpu-debug" 
```

Make sure that LibTorch path matches the build configuration.


