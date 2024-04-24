#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

class Logger 
{
public:
  static void Init() 
  { 
    s_Logger = spdlog::stdout_color_mt("Console");
    
    // Print every level including TRACE and DEBUG 
    s_Logger->set_level(spdlog::level::trace);

    s_Logger->trace("hello");
    s_Logger->info("hello");
    s_Logger->debug("hello");
    s_Logger->critical("hello");
  }
private:
  static std::shared_ptr<spdlog::logger> s_Logger;
};

std::shared_ptr<spdlog::logger> Logger::s_Logger;
