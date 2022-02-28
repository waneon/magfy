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

// run magnifier
#if defined(MAGFY_WINDOWS)
bool run(HINSTANCE, Config &);
#else
bool run(const Config &);
#endif

#endif // __CORE_H