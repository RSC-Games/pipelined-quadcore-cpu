#include "src/util/logger.h"


void _log_error(std::string file, int line, std::string err_msg) {
    std::cerr << "\033[31mERROR: [" << file << ":" << std::to_string(line) << "]: " << err_msg << "\033[0m\n";
}

void _log_warning(std::string file, int line, std::string err_msg) {
    std::cerr << "\033[33mWARN: [" << file << ":" << std::to_string(line) << "]: " << err_msg << "\033[0m\n";
}

void _log_info(std::string file, int line, std::string err_msg) {
    std::cerr << "\033[32mINFO: [" << file << ":" << std::to_string(line) << "]: " << err_msg << "\033[0m\n";
}

void _log_debug(std::string file, int line, std::string err_msg) {
    std::cerr << "\033[34mDEBUG: [" << file << ":" << std::to_string(line) << "]: " << err_msg << "\033[0m\n";
}