#include "Log.h"

#ifdef NDEBUG
  #define CORE_ASSERT
  #define ASSERT
#else
  #define CORE_ASSERT(PREDICATE, ...) if(!(PREDICATE)) { CORE_CRITICAL("Assertion not verified in {} at line {}", __FILE__, __LINE__); CORE_CRITICAL(__VA_ARGS__); std::abort(); }
  #define ASSERT(PREDICATE, ...) if(!(PREDICATE)) { CRITICAL("Assertion not verified in {} at line {}", __FILE__, __LINE__); CRITICAL(__VA_ARGS__); std::abort(); }
#endif