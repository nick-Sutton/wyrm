# Wyrm ![version](https://img.shields.io/badge/version-0.1.0-blue)
Wyrm is a NetNet client for OptiTrack Camera systems. The Wyrm daemon simplifies interacting with OptiTrack systems by managing networking and allowing applications to retrieved data from their OptiTrack system using Zenoh Pub/Sub.

## Requirments
### Compilers and CMake
wyrm supports both the [GCC](https://gcc.gnu.org/) and [Clang](https://clang.llvm.org/) compiler tool chains and targets the [C++ 17](https://en.cppreference.com/cpp/17) standard. wyrm also uses [CMake](https://cmake.org/) as its build system.

**Ubuntu/Debian (GCC)**
```shell
sudo apt update
sudo apt install build-essential make cmake tar curl zip unzip git
```

**Fedora (GCC)**
```shell
sudo dnf update
sudo dnf install gcc gcc-c++ make cmake tar curl zip unzip git
```

### NatNet SDK
[NatNet](https://optitrack.com/software/natnet-sdk) is a Software library developed by OptiTrack, and is used to create client applications for OptiTrack camera systems.

1. Install the [NatNet SDK](https://optitrack.com/software/natnet-sdk) for your OS.
2. Extract the archive to the NatNetSDK folder:
    ```shell
    sudo mkdir ~/NatNetSDK
    sudo tar -xf Downloads/tar_archive_name.tar -C ~/NatNetSDK
    echo 'export NATNET_SDK_ROOT=~/NatNetSDK' >> ~/.bashrc
    source ~/.bashrc
    ```

### Zenoh
[Zenoh](https://zenoh.io/) is a communication protocal, and software stack that wyrm uses to publish data from the NatNet client. To install Zenoh C++ follow these [instructions](https://github.com/eclipse-zenoh/zenoh-cpp).

## Installing wyrm
```shell
git clone git@github.com:nick-Sutton/wyrm.git
```

## Building
In the wyrm root directory run:
```shell
cmake --preset default # or clang
cmake --build build
```


# Starting the Daemon

# Using wyrm in your project
## C++ Example
## Python Example
## ROS2 Example