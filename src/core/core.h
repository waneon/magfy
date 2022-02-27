#ifndef __CORE_H
#define __CORE_H

#include <string>

#include "Config.h"

// return the setting file path
std::string get_config_file();

// run magnifier
bool run(const Config &);

#endif // __CORE_H