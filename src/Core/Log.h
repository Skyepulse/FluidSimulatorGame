#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/sink.h>
#include <spdlog/sinks/ansicolor_sink.h>

#include <memory>

class Logger 
{
public:
  static void Init();
  static const void Test();

  static const std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_CoreLogger; }
  static const std::shared_ptr<spdlog::logger> GetLogger() { return s_Logger; }
private:
  static std::shared_ptr<spdlog::logger> s_CoreLogger;
  static std::shared_ptr<spdlog::logger> s_Logger;
};

#ifdef NDEBUG
  #define CORE_TRACE
  #define CORE_DEBUG
  #define CORE_INFO
  #define CORE_WARN
  #define CORE_ERROR
  #define CORE_CRITICAL

  #define TRACE
  #define DEBUG
  #define INFO
  #define WARN
  #define ERROR
  #define CRITICAL
#else
  #define CORE_TRACE(...)    Logger::GetCoreLogger()->trace(__VA_ARGS__);
  #define CORE_DEBUG(...)    Logger::GetCoreLogger()->debug(__VA_ARGS__);
  #define CORE_INFO(...)     Logger::GetCoreLogger()->info(__VA_ARGS__);
  #define CORE_WARN(...)     Logger::GetCoreLogger()->warn(__VA_ARGS__);
  #define CORE_ERROR(...)    Logger::GetCoreLogger()->error(__VA_ARGS__);
  #define CORE_CRITICAL(...) Logger::GetCoreLogger()->critical(__VA_ARGS__);

  #define TRACE(...)        Logger::GetLogger()->trace(__VA_ARGS__);
  #define DEBUG(...)        Logger::GetLogger()->debug(__VA_ARGS__);
  #define INFO(...)         Logger::GetLogger()->info(__VA_ARGS__);
  #define WARN(...)         Logger::GetLogger()->warn(__VA_ARGS__);
  #define ERROR(...)        Logger::GetLogger()->error(__VA_ARGS__);
  #define CRITICAL(...)     Logger::GetLogger()->critical(__VA_ARGS__);
#endif