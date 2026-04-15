#ifndef __core_config_h
#define __core_config_h

// Enable all logs. Significantly slows down emulation.
#define VERBOSE_MODE                1
#if VERBOSE_MODE
#define VERBOSE_CPU                 1
#define VERBOSE_MMU                 0
#define VERBOSE_MEMORY              1
#define VERBOSE_EXCEPTIONS          1
#else
#endif

// Enable long filepath names in logs. If disabled, uses the GCC-specific
// __FILE_NAME__ instead of __FILE__
#define USE_LONG_PATHS_IN_LOGS      0

#endif  // __core_config_h