#include "Log.h"

#ifdef NDEBUG
  #define CORE_ASSERT
#else
  #define CORE_ASSERT(PREDICATE, ...) if(!(PREDICATE)) { CORE_CRITICAL("Assertion not verified in {} at line {}", __FILE__, __LINE__); CORE_CRITICAL(__VA_ARGS__); std::abort(); }
#endif