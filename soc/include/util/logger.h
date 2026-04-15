#include <iostream>
#include <string>
#include "config.h"

#ifndef INC_LOGGER_H
#define INC_LOGGER_H

#if USE_LONG_PATHS_IN_LOGS
#define _FNAME_MACRO __FILE__
#else
#define _FNAME_MACRO __FILE_NAME__
#endif

#define LOG_ERROR(err_msg) _log_error(_FNAME_MACRO, __LINE__, err_msg)
#define LOG_WARNING(err_msg) _log_warning(_FNAME_MACRO, __LINE__, err_msg)
#define LOG_INFO(err_msg) _log_info(_FNAME_MACRO, __LINE__, err_msg)
#define LOG_DEBUG(err_msg) _log_debug(_FNAME_MACRO, __LINE__, err_msg)

void _log_error(std::string file, int line, std::string err_msg);
void _log_warning(std::string file, int line, std::string err_msg);
void _log_info(std::string file, int line, std::string err_msg);
void _log_debug(std::string file, int line, std::string err_msg);

#endif  // INC_LOGGER_H