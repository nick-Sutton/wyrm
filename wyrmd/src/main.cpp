#include <csignal>
#include <atomic>
#include <iostream>
#include <sstream>
#include <fmt/format.h>
#include <fmt/color.h>
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "zenoh.hxx"
#include <wyrm/types.hpp> 
#include <wyrm/serialization.hpp>
#include <wyrm/keys.hpp>

#include "io.hpp"
#include "net.hpp"
#include "context.hpp"

static std::atomic<bool> g_running{true};
static void signal_handler(int) {
    g_running = false;
}

int main(int argc, char* argv[]) {
    IOHandler io(argc, argv);

    try {
        // Setup and connect to NatNet server
        WyrmConfig wyrm_cfg = io.ParseMotiveConfig();
        io.LogMessage("Creating Client", INFO);
        NatNetClient client{};
        ConnectToServer(client, wyrm_cfg);

        io.LogMessage(fmt::format("Connected to Server at {}!", wyrm_cfg.server_address), SUCCESS);
        io.LogConfig(wyrm_cfg);

        io.LogMessage("Configuring Wyrm Context", INFO);

        // Setup Zenoh Session
        auto config = zenoh::Config::create_default();
        auto session = zenoh::Session::open(std::move(config));

        WyrmContext wyrm_ctx{};
        wyrm_ctx.session = &session;

        // Get Data Descriptions from server and build Rigid Body name table
        sDataDescriptions* descriptions = nullptr;
        client.GetDataDescriptionList(&descriptions);
        BuildDescriptionTable(descriptions, wyrm_ctx);

        // Create a queryable for data descriptions
        auto queryable = wyrm_ctx.session->declare_queryable(
            WyrmDescKeyexpr,
            [&wyrm_ctx](const zenoh::Query& query) {
                std::lock_guard<std::mutex> lock(wyrm_ctx.descriptions_mutex);
                auto payload = SerializeDescriptions(wyrm_ctx.descriptions);
                query.reply(
                    WyrmDescKeyexpr,
                    zenoh::Bytes(std::move(payload))
                );
            },
            zenoh::closures::none  // on_drop handler
        );

        std::signal(SIGINT,  signal_handler);
        std::signal(SIGTERM, signal_handler);

        // Attach frame callback
        client.SetFrameReceivedCallback(frame_callback, &wyrm_ctx);

        io.LogMessage("Wyrm Context Configured", SUCCESS);

        while (g_running) {
            // Wait to get the lock on the data buffer
            std::unique_lock<std::mutex> lock(wyrm_ctx.buffer_mutex);
            wyrm_ctx.buffer_cv.wait(lock, [&]{
                return !wyrm_ctx.frame_buffer.empty() || !g_running;
            });

            if (!g_running) break;

            // Get the first frame in the buffer
            WyrmFrame frame = std::move(wyrm_ctx.frame_buffer.front());
            wyrm_ctx.frame_buffer.pop(); // remove it from the queue
            lock.unlock();

            // Check for model list change before publishing
            if (frame.model_list_changed) {
                sDataDescriptions* desc = nullptr;
                client.GetDataDescriptionList(&desc); // since this is a blocking call we need to do it outside the lock

                WyrmContext temp_ctx{};
                BuildDescriptionTable(desc, temp_ctx); // build into temp

                {
                    std::lock_guard<std::mutex> desc_lock(wyrm_ctx.descriptions_mutex);
                    wyrm_ctx.descriptions = std::move(temp_ctx.descriptions); // swap under lock
                }
                io.LogMessage("Model list changed, descriptions updated.", INFO);
            }

            // Send payload over Zenoh
            auto payload = SerializeFrame(frame);
            session.put(WyrmFrameKeyexpr,
                        zenoh::Bytes(std::move(payload))
                    );
        }

        client.Disconnect();
        io.LogMessage("Disconnected From Server!", SUCCESS);

    } catch (const zenoh::ZException& e) {
        io.LogMessage(e.what(), EXCEPTION);
        return EXIT_FAILURE;
    } catch (const std::runtime_error& e) {
        io.LogMessage(e.what(), RUNTIME_ERROR);
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        io.LogMessage(e.what(), EXCEPTION);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}