#pragma once

#include <string>
#include <wyrm/types.hpp> 
#include "NatNetTypes.h"
#include "NatNetClient.h"
#include "context.hpp"

inline constexpr std::string_view SUCCESS = "Success";
inline constexpr std::string_view INFO = "Info";
inline constexpr std::string_view RUNTIME_ERROR = "Runtime Error";
inline constexpr std::string_view EXCEPTION = "Exception";

class IOHandler {
 private:
    bool        printing = false;
    bool        logging  = false;
    std::string log_path;
    std::string log_dir;
    std::string config_path;

 public:
    IOHandler(int argc, char* argv[]);
    bool IsPrinting() const;
    void LogMessage(const std::string& msg, const std::string_view = INFO) const;
    void LogConfig(const WyrmConfig& cfg) const;
    WyrmConfig ParseMotiveConfig();

 private:
    std::string GenerateLogPath(const std::string& log_dir) const;
};