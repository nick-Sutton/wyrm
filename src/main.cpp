#include <iostream>
#include <sstream>
#include <fmt/format.h>
#include <fmt/color.h>
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "zenoh.hxx"
#include "include/types.hpp"
#include "include/io.hpp"

using namespace zenoh;


void connectToServer(NatNetClient& client, const WyrmConfig wyrmCfg) {
    // Connect to Server
    switch (client.Connect(wyrmCfg.toNatNet())) {
        case ErrorCode_OK:                  break;
        case ErrorCode_Network:             throw std::runtime_error("Unable to Connect to Server.");
        case ErrorCode_InvalidArgument:     throw std::runtime_error("Invalid connection params.");
        case ErrorCode_InvalidOperation:    throw std::runtime_error("Invalid operation.");
        default:                            throw std::runtime_error("Unknown NatNet error.");
    }

    // Test Server Connection
    sServerDescription serverDesc;
    client.GetServerDescription(&serverDesc);

    if (!serverDesc.bConnectionInfoValid) {
        throw std::runtime_error("Invalid connection params. Server predates NatNet 3.0");
    }

    if (!serverDesc.HostPresent) {
        throw std::runtime_error("Unable to Connect to Server. Host not present.");
    }

    // Check that the connection types match
    if (serverDesc.ConnectionMulticast && 
        wyrmCfg.connectionType != ConnectionType_Multicast) {
            throw std::runtime_error("Invalid connection params. Connection Types do not match.");
    }

    if (!serverDesc.ConnectionMulticast && 
        wyrmCfg.connectionType != ConnectionType_Unicast) {
            throw std::runtime_error("Invalid connection params. Connection Types do not match.");
    }

    // Convert serverDesc IP to a c++ string
    std::ostringstream ipOss;
    ipOss << static_cast<int>(serverDesc.HostComputerAddress[0]) << '.'
        << static_cast<int>(serverDesc.HostComputerAddress[1]) << '.'
        << static_cast<int>(serverDesc.HostComputerAddress[2]) << '.'
        << static_cast<int>(serverDesc.HostComputerAddress[3]);

    std::string ipString = ipOss.str();

    if (ipString != wyrmCfg.serverAddress) {
        throw std::runtime_error("Unable to Connect to Server. Host IP Addresses do not Match.");
    }

    // Convert serverDesc Multicast Id to a c++ string
    std::ostringstream mOss;
    mOss << static_cast<int>(serverDesc.ConnectionMulticastAddress[0]) << '.'
        << static_cast<int>(serverDesc.ConnectionMulticastAddress[1]) << '.'
        << static_cast<int>(serverDesc.ConnectionMulticastAddress[2]) << '.'
        << static_cast<int>(serverDesc.ConnectionMulticastAddress[3]);

    std::string multicastString = mOss.str();

    if (multicastString != wyrmCfg.multicastAddress) {
        throw std::runtime_error("Unable to Connect to Server. Multicast Addresses do not match.");
    }
    
    if (serverDesc.ConnectionDataPort != wyrmCfg.serverDataPort) {
        throw std::runtime_error("Invalid connection params. Data ports do not match.");
    }
}

int main(int argc, char *argv[]) {
    try {

        IOHandler io(argc, argv);

        // Read config file
        WyrmConfig wyrmCfg = parseMotiveConfig();

        // Create a client
        NatNetClient client;
        connectToServer(client, wyrmCfg);

        io.logConfig(wyrmCfg);
        io.logMessage(fmt::format("[Success] Connected to Server at {}!", wyrmCfg.serverAddress));

        // Get Data Descriptions from the server
        

        // Connect callbacks
        // TODO

        // Configure Zenoh Session
        init_log_from_env_or("error");
        Config cfg = Config::create_default();
        auto session = Session::open(std::move(cfg));

        // Create a publisher for send data
        auto pub = session.declare_publisher(KeyExpr(wyrm_pub_keyexpr))

        // Create a subscriber to recieve commands
        // auto sub = session.declare_subscriber(wyrm_pub_keyexpr, &data_handler, closures::none);




        // 1. In Mocap Callback -> Put data into a buffer
        // 2. Loop forever, take data out of buffer -> Package message for Zenoh -> call put on zenoh session.

        while (true) {
            // request data from server via callback (callback just puts data in the data buffer)

            // Process a packet of data from the buffer

            // Send data over Zenoh session
        }
        // Loop to receive data from server
        //  Read data from server
        //  Package data for Zenoh
        //  Publish data to Zenoh
        //  Gracefull cleanup on CRTL-C

        // Disconnect from the server
        client.Disconnect();
        io.logMessage("[Success] Disconnected From Server!");

    } catch (const std::runtime_error& e) {
        fmt::print(stderr, bg(fmt::color::coral), "[Runtime error] {}\n", e.what());
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        fmt::print(stderr, bg(fmt::color::crimson), "[Exception] {}\n", e.what());
        return EXIT_FAILURE;
    } catch (ZException e) {
        fmt::print(stderr, bg(fmt::color::crimson), "[Zenoh Error] {}\n", e.what());
    }
    return EXIT_SUCCESS;
}
