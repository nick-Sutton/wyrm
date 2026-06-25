/**
 * Copyright [2026] [Nicholas Sutton]
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#pragma once

#include <string>
#include <wyrm/aliases.hpp> 
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "context.hpp"

inline constexpr std::string_view SUCCESS = "Success";
inline constexpr std::string_view INFO = "Info";
inline constexpr std::string_view RUNTIME_ERROR = "Runtime Error";
inline constexpr std::string_view EXCEPTION = "Exception";

/**
 * An IOHandler stores data related to performing IO, configuration files, and CLI params
 */
class IOHandler {
 private:
   bool        printing = false;
   bool        logging  = false;
   std::string log_path;
   std::string log_dir;
   std::string config_path;

 public:

   /**
    * Constructor for the IOHandler Class
    * @param argc number of CLI args
    * @param argv an array of CLI arguments
    */
   IOHandler(int argc, char* argv[]);

   /**
    * Returns true if Wyrm should print logs to the terminal
    * @return true if the printing flag is set
    */
   bool IsPrinting() const;

   /**
    * Logs a message and its information level.
    * @param msg message to log
    * @param level the type of message being logged
    */
   void LogMessage(const std::string& msg, const std::string_view level = INFO) const;

   /**
    * Logs the provided Wyrm configuration.
    * @param cfg configuration to log
    */
   void LogConfig(const WyrmConfig& cfg) const;

   /**
    * Parses the wyrm configuration file and saves its contents to a WyrmConfig struct.
    * @return a WyrmConfig struct that stores the config settings
    */
   WyrmConfig ParseMotiveConfig();

 private:
   /**
    * Creates a Log file in the specified log directory
    * @param log_dir path to the log directory
    * @return the path to the log file
    */
   std::string GenerateLogPath(const std::string& log_dir) const;
};