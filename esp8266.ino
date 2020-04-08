#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "config.h"
#include "global.h"

ESP8266WebServer httpServer(HTTP_PORT);
FS* filesystem = &LittleFS;
ESP8266HTTPUpdateServer httpUpdater;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.print("Firmware version: ");
  Serial.println(FIRMWARE_VERSION);

  filesystem->begin();

  // Show SPIFFS information
  SPIFFSInformation();

  // prepare GPIO2
  pinMode(LED_PIN, OUTPUT);
  startBlynk();
  setPinState(LED_PIN, LOW);

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  // Wait for connection
  Serial.println();
  Serial.print("Connecting to '");
  Serial.print(STASSID);
  Serial.print("'");

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    ESP.restart();
  }

  // Show connection information
  ConnectionInformation();

  /*
   * The "/files" route section start
   *
   * This section are worth delete after setting up the device
  */
  httpServer.on("/files", HTTP_POST, []() {
    httpServer.send(200, "text/plain", "");
  }, routeFileUpload);
  httpServer.on("/files", HTTP_DELETE, routeFileDelete);
  httpServer.on("/files", HTTP_GET, routeFileList);
  /*
   * The "/files" route section end
  */

  httpServer.on("/health", HTTP_GET, routeHealth);
  httpServer.on("/gpio", HTTP_GET, routeGetPinState);
  httpServer.on("/gpio", HTTP_POST, routeSetPinState);

  httpServer.onNotFound([]() {
    if (!httpServer.authenticate(AUTH_USERNAME, AUTH_PASSWORD)) {
      return httpServer.requestAuthentication();
    }

    if (!handleFileRead(httpServer.uri())) {
      httpServer.send(404, "text/plain", "File Not Found");
    }
  });

  httpUpdater.setup(&httpServer, UPDATE_PATH, UPDATE_USERNAME, UPDATE_PASSWORD);
  httpServer.begin();

  // Show server information
  ServerInformation();
}

void loop() {
  MDNS.update();
  httpServer.handleClient();
}