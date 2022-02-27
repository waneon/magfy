#ifndef __MAGNIFIERS_H
#define __MAGNIFIERS_H

#if defined(MAGFY_WINDOWS)
#include "windows/Magnifier.h"
#elif defined(MAGFY_X11)
#include "gnome/Magnifier.h"
#endif

#endif // __MAGNIFIERS_H