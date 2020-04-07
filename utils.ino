void setPinState(byte pin, byte state) {
  digitalWrite(pin, !state);
}

byte getPinState(byte pin) {
  return digitalRead(pin);
}

void startBlynk() {
  for (uint8_t i = 1; i < 7; i++) {
    setPinState(LED_PIN, i % 2 == 0 ? LOW : HIGH);
    delay(500);
  }
}

void SPIFFSInformation() {
  Dir dir = filesystem->openDir("/");
  Serial.println("SPIFFS information:");

  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("%s [%s]\n", fileName.c_str(), formatBytes(fileSize).c_str());
    fileName = String();
  }

  Serial.println();
}

void ConnectionInformation() {
  Serial.println();
  Serial.print("Connected to '");
  Serial.print(WiFi.SSID());
  Serial.println("'");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(HOSTNAME, WiFi.localIP())) {
    Serial.print("Open http://");
    Serial.print(HOSTNAME);
    Serial.println(".in your browser to see it working");
  }
}

String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) {
  if (httpServer.hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}

bool handleFileRead(String path) {
  if (path.endsWith("/")) {
    path += "index.html";
  }

  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";

  if (filesystem->exists(pathWithGz) || filesystem->exists(path)) {
    if (filesystem->exists(pathWithGz)) {
      path += ".gz";
    }

    File file = filesystem->open(path, "r");
    httpServer.streamFile(file, contentType);
    file.close();

    return true;
  }

  return false;
}
