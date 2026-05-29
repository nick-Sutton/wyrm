#pragma once

struct CmdArgs {
    bool logging = false;
    bool printing = false;
};

struct WyrmConfig {
    uint16_t serverCommandPort;
    uint16_t serverDataPort;
    std::string serverAddress;
    std::string localAddress;
    std::string multicastAddress;
    ConnectionType connectionType;

    // NatNet SDK parameter Struct
    sNatNetClientConnectParams toNatNet() const {
        sNatNetClientConnectParams p;
        p.serverCommandPort = serverCommandPort;
        p.serverDataPort    = serverDataPort;
        p.serverAddress     = serverAddress.c_str();
        p.localAddress      = localAddress.c_str();
        p.multicastAddress  = multicastAddress.c_str();
        p.connectionType    = connectionType;
        return p;
    }
};