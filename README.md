# Wyrm ![version](https://img.shields.io/badge/version-1.0.0-blue)
Wyrm is a NetNet client for OptiTrack Camera systems. The Wyrm daemon simplifies interacting with OptiTrack systems by managing networking and allowing applications to retrieved data from their OptiTrack system using Zenoh Pub/Sub.

## Installation
#### Daemon and C++ Library
```shell
git clone https://github.com/nick-Sutton/wyrm.git
```
Install the [NatNet SDK](https://optitrack.com/software/natnet-sdk) to your ~/Downloads folder.

Wyrm has an installtion script to automate the installation process. After cloning the Wrym repo, run the following commands:
```shell
chmod +x ./install.sh
./install.sh
source ~/.bashrc
```
If you would prefer to install the Wyrm dependencies manually use the following [instructions](https://github.com/nick-Sutton/wyrm/blob/main/docs/manual.md).

#### Build
```shell
cd wyrm
cmake --preset gcc
cmake --build build
cmake --install build --prefix ~/.local
```

#### Running C++ Tests
If you want to run the C++ test suite for Wyrm use:
```shell
cmake --preset gcc -DWYRM_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

To use the Wyrm C++ library put these lines in your CMake file:
```txt
find_package(wyrm-cpp REQUIRED)
target_link_libraries(my_target PRIVATE wyrm::wyrm-cpp)
```

#### Python
```shell
pip install wyrmpy # or uv add wyrmpy
```

# Using Wyrm
Run the wyrmd daemon:
```shell
./build/wyrmd/wyrmd # -p for printing
```
## C++ Example
```cpp
#include <iostream>
#include <zenoh.hxx>
#include <wyrm/keys.hpp>
#include <wyrm/serialization.hpp>

int main() {
    auto session = zenoh::Session::open(zenoh::Config::create_default());

    // Fetch descriptions once
    for (auto& reply : session.get(WyrmDescKeyexpr, "")) {
        if (reply.is_ok()) {
            auto descriptions = DeserializeDescriptions(reply.get_ok().get_payload().as_vector());
            for (const auto& [id, desc] : descriptions)
                std::cout << "description: " << desc.name << "\n";
        }
    }

    // Subscribe to frames
    auto sub = session.declare_subscriber(WyrmFrameKeyexpr, [](const zenoh::Sample& sample) {
        auto frame = DeserializeFrame(sample.get_payload().as_vector());
        std::cout << "frame " << frame.frame_id << ": " << frame.body_count << " bodies\n";
        for (const auto& body : frame.bodies)
            std::cout << "  " << body.name << ": ("
                      << body.x << ", " << body.y << ", " << body.z << ")\n";
    });

    std::cin.get();
}
```
## Python Example
```python
import zenoh
from wyrmpy import deserialize_frame, deserialize_descriptions
from wyrmpy.keys import WYRM_FRAME_KEYEXPR, WYRM_DESC_KEYEXPR

with zenoh.open(zenoh.Config()) as session:
    # Fetch descriptions
    for reply in session.get(WYRM_DESC_KEYEXPR):
        descriptions = deserialize_descriptions(bytes(reply.ok.payload))
        for desc in descriptions.values():
            print(desc)

    # Subscribe to frames
    def on_frame(sample):
        frame = deserialize_frame(bytes(sample.payload))
        print(f"frame {frame.frame_id}: {frame.body_count} bodies")
        for body in frame.bodies:
            print(f"  {body.name}: ({body.x:.3f}, {body.y:.3f}, {body.z:.3f})")

    sub = session.declare_subscriber(WYRM_FRAME_KEYEXPR, on_frame)
    input("Press Enter to quit\n")
```
## ROS2 Integration
```python
# Check back soon.
```
