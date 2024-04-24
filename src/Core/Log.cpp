#include "Log.h"

std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
std::shared_ptr<spdlog::logger> Logger::s_Logger;

void Logger::Init()
{ 
  // Create sink and add format for each level
  auto coreSink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
  coreSink->set_color(spdlog::level::trace,    "\033[37m");
  coreSink->set_color(spdlog::level::debug,    "\033[36m");
  coreSink->set_color(spdlog::level::info,     "\033[32m");
  coreSink->set_color(spdlog::level::warn,     "\033[33m");
  coreSink->set_color(spdlog::level::err,      "\033[31m");
  coreSink->set_color(spdlog::level::critical, "\033[1;31m");
  // Create sink list for Logger initialization
  spdlog::sinks_init_list coreSinkList = { coreSink };

  s_CoreLogger = std::make_shared<spdlog::logger>("Core Console", coreSinkList);
  s_CoreLogger->set_pattern("%^[CORE] [%H:%M:%S.%e] [%l] %v%$");
  s_CoreLogger->set_level(spdlog::level::trace);

  // Copy coreSink, must be different from the one in core logger,
  // as set_pattern applies to the pointer coreSink
  auto sink =std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
  sink->set_color(spdlog::level::trace,    "\033[37m");
  sink->set_color(spdlog::level::debug,    "\033[36m");
  sink->set_color(spdlog::level::info,     "\033[32m");
  sink->set_color(spdlog::level::warn,     "\033[33m");
  sink->set_color(spdlog::level::err,      "\033[31m");
  sink->set_color(spdlog::level::critical, "\033[1;31m");
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

