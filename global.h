#include <ESP8266WebServer.h>
#include <FS.h>

#ifndef GLOBAL_DEFINES
#define GLOBAL_DEFINES
extern ESP8266WebServer httpServer;
extern FS* filesystem;
#endif