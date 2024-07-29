#include <iostream>
#include <string>


// Log an error to stderr. This prints the file and line number of the error.
void inline log_error(std::string err_msg) {
    std::cerr << "\033[31mERROR: [" << __FILE__ << ":" << __LINE__ << "]: " << err_msg << "\n";
}