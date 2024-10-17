#ifndef __core_config_h
#define __core_config_h

// Enable all logs. Significantly slows down emulation.
#define VERBOSE_MODE                1
#if VERBOSE_MODE
#define VERBOSE_CPU                 1
#define VERBOSE_MMU                 1
#define VERBOSE_MEMORY              1
#define VERBOSE_EXCEPTIONS          1
#else
#endif

#endif  // __core_config_h