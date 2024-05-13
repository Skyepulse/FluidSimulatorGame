#pragma once

#define CORE_TIME_IN_UNIT(unit) std::chrono::duration_cast<std::chrono::unit>(GetTime().time_since_epoch()).count()

#include <chrono>
#include <math.h>

#ifndef _WIN32
    using ClockType = std::chrono::system_clock;
#else
    using ClockType = std::chrono::high_resolution_clock;
#endif

class Time
{
public:
  enum class TimeUnit { Seconds, Milliseconds, Microseconds, Nanoseconds };

    // Platform-specific clock selection

  static ClockType::time_point GetTime() { return m_Clock.now(); }  
  static double GetMillis()   { return CORE_TIME_IN_UNIT(microseconds)  / 1000.0; }
  static double GetMicros()   { return CORE_TIME_IN_UNIT(nanoseconds)   / 1000.0; }
  static double GetSeconds()  { return CORE_TIME_IN_UNIT(milliseconds)  / 1000.0; }

  static double GetDeltaTime(TimeUnit timeUnit = TimeUnit::Milliseconds) { double temp = m_LastSampledMilliseconds; m_LastSampledMilliseconds = GetMillis(); return ConvertToTimeUnit(m_LastSampledMilliseconds - temp, TimeUnit::Milliseconds, timeUnit); }
private:
  static double ConvertToTimeUnit(double time, TimeUnit fromUnit, TimeUnit toUnit) 
  {
    double factor = pow(10.0, 3.0 * ((int)toUnit - (int)fromUnit));
    return time * factor;
  }
private:
  static ClockType m_Clock;
  static double m_LastSampledMilliseconds;
};

ClockType Time::m_Clock;
double Time::m_LastSampledMilliseconds;