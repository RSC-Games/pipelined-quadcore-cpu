rem Don't use this script for building forever! It's a stopgap until CMake is used.
g++ -Isoc soc/src/main.cpp soc/src/soc.cpp soc/src/core/core.cpp soc/src/core/memory.cpp soc/src/util/logger.cpp