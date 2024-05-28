#include "Time.h"

ClockType Time::m_Clock;
double Time::m_LastSampledMilliseconds;

double Time::GetDeltaTime(TimeUnit timeUnit)
{
  double temp = m_LastSampledMilliseconds;
  m_LastSampledMilliseconds = GetMillis();
  
  return ConvertToTimeUnit(m_LastSampledMilliseconds - temp, TimeUnit::Milliseconds, timeUnit);
}

double Time::ConvertToTimeUnit(double time, TimeUnit fromUnit, TimeUnit toUnit)
{
  double factor = pow(10.0, 3.0 * ((int)toUnit - (int)fromUnit));
  return time * factor;
}
