#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef _WIN32
  #define TRACE_COLOR     0x0007
  #define DEBUG_COLOR     0x0003
  #define INFO_COLOR      0x0002
  #define WARN_COLOR      0x0006
  #define ERR_COLOR       0x0004
  #define CRITICAL_COLOR  0x000C
#else
  #define TRACE_COLOR     "\033[37m" 
  #define DEBUG_COLOR     "\033[36m" 
  #define INFO_COLOR      "\033[32m" 
  #define WARN_COLOR      "\033[33m" 
  #define ERR_COLOR       "\033[31m"
  #define CRITICAL_COLOR  "\033[1;31m" 
#endif

std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::Init()
{ 
  // Create sink and add format for each level
  auto coreSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  coreSink->set_color(spdlog::level::trace,    TRACE_COLOR   );
  coreSink->set_color(spdlog::level::debug,    DEBUG_COLOR   );
  coreSink->set_color(spdlog::level::info,     INFO_COLOR    );
  coreSink->set_color(spdlog::level::warn,     WARN_COLOR    );
  coreSink->set_color(spdlog::level::err,      ERR_COLOR     );
  coreSink->set_color(spdlog::level::critical, CRITICAL_COLOR);
  // Create sink list for Logger initialization
  spdlog::sinks_init_list coreSinkList = { coreSink };

  s_CoreLogger = std::make_shared<spdlog::logger>("Core Console", coreSinkList);
  s_CoreLogger->set_pattern("%^[CORE] [%H:%M:%S.%e] [%l] %v%$");
  s_CoreLogger->set_level(spdlog::level::trace);

  // Copy coreSink, must be different from the one in core logger,
  // as set_pattern applies to the pointer coreSink
  auto sink =std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  sink->set_color(spdlog::level::trace,     TRACE_COLOR   );
  sink->set_color(spdlog::level::debug,     DEBUG_COLOR   );
  sink->set_color(spdlog::level::info,      INFO_COLOR    );
  sink->set_color(spdlog::level::warn,      WARN_COLOR    );
  sink->set_color(spdlog::level::err,       ERR_COLOR     );
  sink->set_color(spdlog::level::critical,  CRITICAL_COLOR); 
  // Create sink list for Logger initialization
  spdlog::sinks_init_list sinkList = { sink };

  s_Logger = std::make_shared<spdlog::logger>("Core Console", sinkList);
  s_Logger->set_pattern("%^[%H:%M:%S.%e] [%l] %v%$");
  s_Logger->set_level(spdlog::level::trace);
}


const void Logger::Test()
{
  s_CoreLogger->trace("Trace");
  s_CoreLogger->debug("Debug");
  s_CoreLogger->info("Info");
  s_CoreLogger->warn("Warn");
  s_CoreLogger->error("Error");
  s_CoreLogger->critical("Critical");

  s_Logger->trace("Trace");
  s_Logger->debug("Debug");
  s_Logger->info("Info");
  s_Logger->warn("Warn");
  s_Logger->error("Error");
  s_Logger->critical("Critical");
}

