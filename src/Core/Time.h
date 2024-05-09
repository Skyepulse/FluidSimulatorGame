#pragma once

#define CORE_TIME_IN_UNIT(unit) std::chrono::duration_cast<std::chrono::unit>(GetTime().time_since_epoch()).count()

#include <chrono>

class Time
{
public:
  static std::chrono::_V2::system_clock::time_point GetTime() { return m_Clock.now(); }
  static double GetMillis()   { return CORE_TIME_IN_UNIT(microseconds)  / 1000.0; }
  static double GetMicros()   { return CORE_TIME_IN_UNIT(nanoseconds)   / 1000.0; }
  static double GetSeconds()  { return CORE_TIME_IN_UNIT(milliseconds)  / 1000.0; }

  static double GetDeltaTime() { double temp = m_LastSampledMilliseconds; m_LastSampledMilliseconds = GetMillis(); return m_LastSampledMilliseconds - temp; }

private:
  static std::chrono::high_resolution_clock m_Clock;
  static double m_LastSampledMilliseconds;
};

std::chrono::high_resolution_clock Time::m_Clock;
double Time::m_LastSampledMilliseconds;