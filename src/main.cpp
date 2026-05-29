#include <iostream>
#include <fmt/format.h>
#include <fmt/color.h>
#include <argparse/argparse.hpp>
#include <toml++/toml.hpp>
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "include/types.hpp"

CmdArgs parseArgs(int argc, char *argv[]) {
    argparse::ArgumentParser program("mocapd");
    program.add_argument("-l", "--logging")
        .help("Save logs to a file.")
        .flag();
    program.add_argument("-p", "--printing")
        .help("Print information to the console.")
        .flag();

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << err.what() << "\n";
        std::cerr << program;
        std::exit(EXIT_FAILURE); // TODO: This is bad
    }

    return CmdArgs {
        .logging  = program.get<bool>("--logging"),
        .printing = program.get<bool>("--printing"),
    };
}

WyrmConfig parseMotiveConfig(const std::string& path = "./config/config.toml") {
    toml::table tbl = toml::parse_file(path);

    WyrmConfig cfg;
    cfg.serverCommandPort = tbl["networking"]["command_port"].value<uint16_t>().value_or(0);
    cfg.serverDataPort    = tbl["networking"]["data_port"].value<uint16_t>().value_or(0);
    cfg.serverAddress     = tbl["networking"]["server_address"].value<std::string>().value_or("");
    cfg.localAddress      = tbl["networking"]["client_address"].value<std::string>().value_or("");
    cfg.multicastAddress  = tbl["networking"]["multicast_address"].value<std::string>().value_or("");
    cfg.connectionType    = tbl["networking"]["use_multicast"].value<bool>().value_or(false)
                              ? ConnectionType_Multicast
                              : ConnectionType_Unicast;
    return cfg;
}

int main(int argc, char *argv[]) {
    try {
        // Parse cmd args
        CmdArgs args = parseArgs(argc, argv);

        // Read config file
        WyrmConfig wyrmCfg = parseMotiveConfig();

        if (args.printing) {
            fmt::println("---Wyrm Configuration---");
            fmt::println("Connection Type: {}", 
                wyrmCfg.connectionType == ConnectionType_Multicast ? "Multicast" : "Unicast");
            fmt::println("Server Address: {}", wyrmCfg.serverAddress);
            fmt::println("Client Address: {}", wyrmCfg.localAddress);
        }

        // Create a client
        NatNetClient client;

        // Connect to Server
        switch (client.Connect(wyrmCfg.toNatNet())) {
            case ErrorCode_OK:                  break;
            case ErrorCode_Network:             throw std::runtime_error("Unable to Connect to Server.");
            case ErrorCode_InvalidArgument:     throw std::runtime_error("Invalid connection params.");
            case ErrorCode_InvalidOperation:    throw std::runtime_error("Invalid operation.");
            default:                            throw std::runtime_error("Unknown NatNet error.");
        }

        // Disconnect from the server
        client.Disconnect();

    } catch (const std::runtime_error& e) {
        fmt::print(stderr, bg(fmt::color::coral), "[Runtime error] {}\n", e.what());
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        fmt::print(stderr, bg(fmt::color::crimson), "[Exception] {}\n", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
