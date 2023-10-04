#include <SPI.h>
#include <Seeed_FS.h> // SD card library
#include <secrets.h>

#include "ArduinoJson.h"
#include "SD/Seeed_SD.h"

static const char *ValueGetOrDefault(const char *key, const char *default_value);

static StaticJsonDocument<500> jsonDocConfigFile;

void readConfigFromSDCard()
{
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 16000000)) {
    Serial.println("SD Card Mount Failed");
    return;
  }
  
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
    //Serial.println("JSON file parsed successfully!");
  }
  configFile.close();
}

const char *getWifiSSID()
{
  return ValueGetOrDefault("SSID", secrets_wifi.ssid);
}

const char *getWifiPW()
{
  return ValueGetOrDefault("PW", secrets_wifi.passkey);
}

const char *getWioTerminalID()
{
  return ValueGetOrDefault("ID", "0");
}

const char *getWioTerminalFuntionality()
{
  return ValueGetOrDefault("default_functionality", "");
}

const char *getMQTTBroker(){
  return ValueGetOrDefault("mqtt_bootstrap_broker", secrets_mqtt.broker);
}

const char *getMQTTUser(){
  return ValueGetOrDefault("mqtt_bootstrap_broker_user", secrets_mqtt.user);
} 

const char *getMQTTPW(){
  return ValueGetOrDefault("mqtt_bootstrap_broker_password", secrets_mqtt.password);
}

static const char *ValueGetOrDefault(const char *key, const char *default_value) {
  const char *result = jsonDocConfigFile[key];
  return result ? result : default_value;
}
