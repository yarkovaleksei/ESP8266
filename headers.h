#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <Timer.h>

#ifndef GLOBAL_DEFINES
#define GLOBAL_DEFINES
extern ESP8266WebServer httpServer;
extern FS* filesystem;
extern Timer timer;
#endif