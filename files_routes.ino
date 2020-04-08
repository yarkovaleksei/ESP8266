#include "global.h"

File fsUploadFile;

void routeFileList() {
  StaticJsonDocument<1024> json;
  JsonArray files = json.createNestedArray("files");
  Dir dir = filesystem->openDir("/");

  while (dir.next()) {
    StaticJsonDocument<64> fileItem;
    File entry = dir.openFile("r");

    if (entry.name()[0] == '/') {
      fileItem["name"] = String(&(entry.name()[1]));
    } else {
      fileItem["name"] = String(entry.name());
    }

    fileItem["size"] = entry.size();

    files.add(fileItem);
    entry.close();
  }

  String output;
  serializeJsonPretty(json, output);
  httpServer.send(200, "application/json", output);
  output = String();
}

void routeFileUpload() {
  HTTPUpload& upload = httpServer.upload();
  String filename = upload.filename;

  if (upload.status == UPLOAD_FILE_START) {
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }

    fsUploadFile = filesystem->open(filename, "w");

  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }

  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
    }

    Serial.println("Uploaded file: " + filename + " (size: " + formatBytes(upload.totalSize) + ")");
    filename = String();
  }
}

void routeFileDelete() {
  if (httpServer.args() == 0) {
    return httpServer.send(500, "text/plain", "No file name. Use: \"curl -X DELETE /files?file=file.ext\"");
  }

  String path = httpServer.arg(0);

  if (path == "/") {
    return httpServer.send(500, "text/plain", "BAD PATH");
  }

  if (!filesystem->exists(path)) {
    return httpServer.send(404, "text/plain", "File Not Found");
  }

  filesystem->remove(path);
  Serial.println("\"" + path + "\" has been deleted");
  httpServer.send(200, "text/plain", "");
  path = String();
}