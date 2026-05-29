#include <iostream>
#include <argparse/argparse.hpp>
#include <toml++/toml.hpp>
#include "NatNetTypes.h"
#include "NatNetClient.h"

struct CmdArgs {
    bool logging = false;
    bool printing = false;
};

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

sNatNetClientConnectParams parseMotiveConfig() {

    toml::table tbl
    try {
        tbl = toml::parse_file("./config/config.toml");

        MotiveConfig motiveConfig;
        motiveConfig.camera_freq = tbl["mocap"]["camera_frequency"];
        motiveConfig.serverAddress = tbl["networking"]["server_address"];
        motiveConfig.clientAddress = tbl["networking"]["client_address"];
        motiveConfig.multicastAddress = tbl["networking"]["multicast_address"];
        motiveConfig.commandPort = tbl["networking"]["command_port"];
        motiveConfig.dataPort = tbl["networking"]["data_port"];
        motiveConfig.logDir = tbl["logging"]["log_dir"];

        if (tbl["networking"]["use_multicast"]) {
            motiveConfig.connectionType = ConnectionType_Multicast;
        } else {
            motiveConfig.connectionType = ConnectionType_Unicast;
        }

        return motiveConfig;

    } catch (const toml::parse_error& err) {
        std::cerr << "Failed to Parse:\n" << err << "\n";
        std::exit(EXIT_FAILURE); // TODO: This is bad
    }
}

int createClient(ConnectionType connectionType) {

    // release previous server
    if(g_pClient) {
        g_pClient -> Disconnect();
        delete g_pClient;
    }
    // create NatNet client
    g_pClient = new NatNetClient(connectionType);
}


int main(int argc, char *argv[]) {
    // Parse cmd args
    CmdArgs args = parseArgs(argc, argv);

    // Read config file
    MotiveConfig motiveConfig = parseMotiveConfig();

    std::cout << "Parse Success!" << std::endl;
    return EXIT_SUCCESS;
}
