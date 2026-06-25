# Wyrm ![version](https://img.shields.io/badge/version-1.0.0-blue)
Wyrm is a NetNet client for OptiTrack Camera systems. The Wyrm daemon simplifies interacting with OptiTrack systems by managing networking and allowing applications to retrieved data from their OptiTrack system using CyloneDDS Pub/Sub.

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
To run the C++ example, put the following lines in your CMakeLists.txt file:

```cmake
cmake_minimum_required(VERSION 3.16)
project(wyrm-test)

find_package(CycloneDDS-CXX REQUIRED)
find_package(wyrm-cpp REQUIRED)

add_executable(wyrm_test wyrm_test.cpp)
target_link_libraries(wyrm_test PRIVATE
    wyrm::wyrm-cpp
    CycloneDDS-CXX::ddscxx
)
```

Example: 

```cpp
#include <iostream>
#include "dds/dds.hpp"
#include <wyrm/aliases.hpp>
#include <wyrm/topics.hpp>
#include <thread>
#include <chrono>

using namespace org::eclipse::cyclonedds;

int main() {
    /* ROS_DOMAIN_ID=42 */
    dds::domain::DomainParticipant participant(42);

    /* Descriptions */
    dds::topic::Topic<WyrmDescription> description_topic(participant, WyrmDescriptionTopic);

    dds::sub::qos::DataReaderQos description_reader_qos;
    description_reader_qos << dds::core::policy::Durability::TransientLocal()
             << dds::core::policy::Reliability::Reliable();
    dds::sub::Subscriber subscriber(participant);
    dds::sub::DataReader<WyrmDescription> desc_reader(subscriber, description_topic, description_reader_qos);

    // Delay for reader to discover writer
    dds::core::Duration wait(3, 0); // 3 seconds
    desc_reader.wait_for_historical_data(wait);

    auto descriptions = desc_reader.read();
    for (const auto& sample : descriptions) {
        if (sample.info().valid()) {
            const auto& d = sample.data();
            std::cout << "description id=" << d.id()
                      << " name=" << d.name() << "\n";
        }
    }

    /* Frames */
    dds::topic::Topic<WyrmFrame> frame_topic(participant, WyrmFrameTopic);
    dds::sub::DataReader<WyrmFrame> frame_reader(subscriber, frame_topic);
   
    /* Read Condition for WaitSet */
    dds::sub::cond::ReadCondition read_cond(
        frame_reader,
        dds::sub::status::DataState::new_data()
    );

    /* Waitset */
    dds::core::cond::WaitSet waitset;
    waitset += read_cond;

    std::cout << "Listening for frames, Ctrl+C to quit...\n";

    /* Waitset Loop */
    while (true) {
        try {
            waitset.wait(dds::core::Duration(5, 0));
        } catch (const dds::core::TimeoutError&) {
            std::cout<<"no frames recevied in 5s, waiting...\n";
            continue;
        }
        for (const auto& sample : frame_reader.take()) {
            if (!sample.info().valid()) continue;
            const auto& f = sample.data();
            std::cout << "frame " << f.frame_id()
                      << ": " << f.body_count() << " bodies\n";
            for (const auto& body : f.bodies()) {
                std::cout << "  " << body.name()
                          << ": (" << body.position().x()
                          << ", "  << body.position().y()
                          << ", "  << body.position().z() << ")\n";
            }
        }
    }
}
```
## Python Example
```python
# Check back soon.
```
## ROS2 Integration
```python
# Check back soon.
```

## Connectvity issues
```shell
sudo export CYCLONEDDS_URI="file:///{filepath to wyrm/cyclonedds.xml}"
```
**Highly Recommended : Add above line to ~/.bashrc*

```shell
sudo firewall-cmd --permanent --add-rich-rule='rule family="ipv4" destination address="239.255.0.1" accept'
sudo firewall-cmd --permanent --add-port=7400/udp
```
