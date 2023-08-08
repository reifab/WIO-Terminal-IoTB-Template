#include <SPI.h>
#include <Seeed_FS.h> // SD card library
#include "ArduinoJson.h"
#include "SD/Seeed_SD.h"

StaticJsonDocument<200> jsonDocConfigFile;

void readConfigFromSDCard()
{
  File configFile = SD.open("sys/wioID.json", "r"); // Die SD Karte wurde hier bereits initialisiert in der Funktion initDisplay(). Siehe lib\wio_display\wio_display.cpp. ToDo: SD Karte in eigene Funktion auslagern.
  if (!configFile)
  {
    Serial.println("Failed to open config file!");
    return;
  }

  String buf = configFile.readString();

  DeserializationError error = deserializeJson(jsonDocConfigFile, buf);

  if (error)
  {
    Serial.println("Failed to parse JSON file!");
    Serial.println(error.c_str());
    return;
  }
  else
  {
    Serial.println("JSON file parsed successfully!");
  }
  configFile.close();
}

const char *readIDOfWioTerminal()
{
  return jsonDocConfigFile["ID"];
}
