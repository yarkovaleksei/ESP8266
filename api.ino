void routeHealth() {
  StaticJsonDocument<500> json;
  String output;
  int seconds = millis() / 1000;
  int minutes = seconds / 60;
  int hours = minutes / 60;

  json["firmware"] = FIRMWARE_VERSION;
  json["uptime"]["hours"] = hours;
  json["uptime"]["minutes"] = minutes % 60;
  json["uptime"]["seconds"] = seconds % 60;

  serializeJsonPretty(json, output);
  httpServer.send(200, "application/json", output);
  output = String();
}

void routeGetPinState() {
  if (!httpServer.authenticate(AUTH_USERNAME, AUTH_PASSWORD)) {
    return httpServer.requestAuthentication();
  }

  StaticJsonDocument<64> response;
  String output;

  response["state"] = !getPinState(LED_PIN) ? 1  : 0;
  serializeJsonPretty(response, output);
  httpServer.send(200, "application/json", output);
  output = String();
}

void routeSetPinState() {
  if (!httpServer.authenticate(AUTH_USERNAME, AUTH_PASSWORD)) {
    return httpServer.requestAuthentication();
  }

  if (!httpServer.hasArg("value")) {
    httpServer.send(400, "text/plain", "Argument \"value\" is required");
    return;
  }

  byte value = httpServer.arg("value").toInt();

  if (value != 0 && value != 1) {
    httpServer.send(400, "text/plain", "The request must be in the format \"{\"value\" : 0}\" or \"{\"value\" : 1}\"");
    return;
  }

  StaticJsonDocument<64> response;
  String output;

  response["state"] = value;
  setPinState(LED_PIN, value);
  serializeJsonPretty(response, output);
  httpServer.send(200, "application/json", output);
  output = String();
}
