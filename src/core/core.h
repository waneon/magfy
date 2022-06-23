#ifndef __CORE_H
#define __CORE_H

#include <string>

#if defined(MAGFY_WINDOWS)
#include <windows.h>
#endif

#include "Config.h"

// return the setting file path
std::string get_config_file();

// return the log file path
#if defined(MAGFY_WINDOWS)
std::string get_log_file();
#endif

// raise error message
void error(std::string error_message);

// run magnifier
void run(const Config &config);

#endif // __CORE_H