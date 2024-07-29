#include <iostream>
#include <string>

#define LOG_ERROR(err_msg) _log_error(__FILE__, __LINE__, err_msg)

// Log an error to stderr. This prints the file and line number of the error.
void inline _log_error(std::string file, int line, std::string err_msg) {
    std::cerr << "\033[31mERROR: [" << file << ":" << std::to_string(line) << "]: " << err_msg << "\033[0m\n";
}