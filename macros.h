#ifndef MACROS_H
#define MACROS_H

// Logging
#include "logger.h"

//Logging - macros
#define FLog(msg,type) logger::instance().logmsg(msg,type) //Full log, includes predetermined color coded severity types
#define QLog(msg,color) logger::instance().logmsg(msg,color) //Quick log, only text, but allows custom color




#endif // MACROS_H
