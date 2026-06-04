#include "include/net.hpp"

void ConnectToServer(NatNetClient& client, const WyrmConfig& cfg) {
    switch (client.Connect(cfg.ToNatNet())) {
        case ErrorCode_OK:               break;
        case ErrorCode_Network:          throw std::runtime_error("Unable to Connect to Server");
        case ErrorCode_InvalidArgument:  throw std::runtime_error("Invalid connection params");
        case ErrorCode_InvalidOperation: throw std::runtime_error("Invalid operation");
        default:                         throw std::runtime_error("Unknown NatNet error");
    }

    sServerDescription server_desc;
    client.GetServerDescription(&server_desc);

    if (!server_desc.bConnectionInfoValid)
        throw std::runtime_error("Invalid connection params. Server predates NatNet 3.0");
    if (!server_desc.HostPresent)
        throw std::runtime_error("Unable to Connect to Server. Host not present");

    if (server_desc.ConnectionMulticast &&
        cfg.connection_type != ConnectionType_Multicast)
            throw std::runtime_error("Invalid connection params. Connection Types do not match");

    if (!server_desc.ConnectionMulticast &&
        cfg.connection_type != ConnectionType_Unicast)
            throw std::runtime_error("Invalid connection params. Connection Types do not match");

    std::ostringstream ip_oss;
    ip_oss << static_cast<int>(server_desc.HostComputerAddress[0]) << '.'
           << static_cast<int>(server_desc.HostComputerAddress[1]) << '.'
           << static_cast<int>(server_desc.HostComputerAddress[2]) << '.'
           << static_cast<int>(server_desc.HostComputerAddress[3]);

    if (ip_oss.str() != cfg.server_address)
        throw std::runtime_error("Unable to Connect to Server. Host IP Addresses do not match");

    std::ostringstream mc_oss;
    mc_oss << static_cast<int>(server_desc.ConnectionMulticastAddress[0]) << '.'
           << static_cast<int>(server_desc.ConnectionMulticastAddress[1]) << '.'
           << static_cast<int>(server_desc.ConnectionMulticastAddress[2]) << '.'
           << static_cast<int>(server_desc.ConnectionMulticastAddress[3]);

    if (mc_oss.str() != cfg.multicast_address)
        throw std::runtime_error("Unable to Connect to Server. Multicast Addresses do not match");

    if (server_desc.ConnectionDataPort != cfg.server_data_port)
        throw std::runtime_error("Invalid connection params. Data ports do not match.");
}

void NATNET_CALLCONV frame_callback(sFrameOfMocapData* data, void* user_data) {
    auto* ctx = static_cast<WyrmContext*>(user_data);
    WyrmFrame frame;
    {
        // Aquire Description Lock
        std::lock_guard<std::mutex> desc_lock(ctx->descriptions_mutex);
        frame = ToWyrmFrame(*data, ctx->descriptions);
    }
    {
        // Aquire Data buffer lock
        std::lock_guard<std::mutex> lock(ctx->buffer_mutex);
        ctx->frame_buffer.push(std::move(frame));
    }
    // Notify other threads that we are finished
    ctx->buffer_cv.notify_one();
}