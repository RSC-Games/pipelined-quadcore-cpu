#include <iostream>
#include <string>

#define LOG_ERROR(err_msg) _log_error(__FILE__, __LINE__, err_msg)
#define LOG_WARNING(err_msg) _log_warning(__FILE__, __LINE__, err_msg)
#define LOG_INFO(err_msg) _log_info(__FILE__, __LINE__, err_msg)
#define LOG_DEBUG(err_msg) _log_debug(__FILE__, __LINE__, err_msg)

void _log_error(std::string file, int line, std::string err_msg);
void _log_warning(std::string file, int line, std::string err_msg);
void _log_info(std::string file, int line, std::string err_msg);
void _log_debug(std::string file, int line, std::string err_msg);