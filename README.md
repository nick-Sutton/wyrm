# Wyrm ![version](https://img.shields.io/badge/version-0.1.0-blue)
Wyrm is a NetNet client for OptiTrack Camera systems. The Wyrm daemon simplifies interacting with OptiTrack systems by managing networking and allowing applications to retrieved data from their OptiTrack system using Zenoh Pub/Sub.

## Installation
```shell
git clone https://github.com/nick-Sutton/wyrm.git
```

Wyrm has an installtion script to automate the installation process. After cloning the Wrym repo, run the following commands:
```shell
chmod +x ./install.sh
./install.sh
source ~/.bashrc
```
If you would prefer to install the Wyrm dependencies manually use the following [instructions](https://github.com/nick-Sutton/wyrm/blob/main/docs/manual.md).

## Building
In the wyrm root directory run:
```shell
cmake --preset default # or clang
cmake --build build
```

# Registering the Daemon
# Using Wyrm
Run the wyrmd daemon:
```shell
./build/wyrmd # --printing or -p to see logs
```
## C++ Example
```cpp
// Check back soon
```
## Python Example
```python
# Check back soon
```
## ROS2 Integration
