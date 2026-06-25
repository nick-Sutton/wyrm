/**
 * Copyright [2026] [Nicholas Sutton]
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include <csignal>
#include <atomic>
#include <iostream>
#include <sstream>
#include <fmt/format.h>
#include <fmt/color.h>
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "dds/dds.hpp"

#include <wyrm/aliases.hpp> 
#include <wyrm/topics.hpp>

#include "io.hpp"
#include "net.hpp"
#include "context.hpp"

using namespace org::eclipse::cyclonedds;

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

        /* Domain and Publisher (ROS_DOMAIN_ID = 42)*/
        dds::domain::DomainParticipant participant(42);
        dds::pub::Publisher publisher(participant);

        /* Description topic/writer */
        dds::topic::Topic<WyrmDescription> description_topic(participant, WyrmDescriptionTopic);

       /* TRANSIENT_LOCAL so late-joining subscribers still receive current description set without needing to query for it. */
        dds::pub::qos::DataWriterQos description_writer_qos = publisher.default_datawriter_qos()
            << dds::core::policy::Durability::TransientLocal()
            << dds::core::policy::Reliability::Reliable();
        dds::pub::DataWriter<WyrmDescription> description_writer(publisher, description_topic, description_writer_qos);
        
        /* Frame topic/writer, default QOS */
        dds::topic::Topic<WyrmFrame> frame_topic(participant, WyrmFrameTopic);
        dds::pub::DataWriter<WyrmFrame> frame_writer(publisher, frame_topic);

        WyrmContext wyrm_ctx{};
        wyrm_ctx.description_writer = &description_writer;
        wyrm_ctx.frame_writer = &frame_writer;

        // Get Data Descriptions from server and build Rigid Body Description table
        sDataDescriptions* descriptions = nullptr;
        client.GetDataDescriptionList(&descriptions);
        BuildDescriptionTable(descriptions, wyrm_ctx);

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
            // The print statement was here
            if (!g_running) break; // Break if a signal is received 

            // Get the first frame in the buffer
            WyrmFrame frame = std::move(wyrm_ctx.frame_buffer.front());
            wyrm_ctx.frame_buffer.pop(); // remove it from the queue
            lock.unlock();

            // Check for model list change before publishing
            if (frame.model_list_changed()) {
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

            frame_writer.write(frame);
        }

        client.Disconnect();

    } catch (const dds::core::Exception& e) {
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