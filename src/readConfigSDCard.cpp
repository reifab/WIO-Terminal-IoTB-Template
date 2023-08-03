#include <SPI.h>
#include <Seeed_FS.h>            // SD card library
#include "ArduinoJson.h"
#include "SD/Seeed_SD.h"

static int sd_card_status = 0;

void setupSD(){
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 16000000)) {
    sd_card_status = 0;   // no SD card
  }
  else
  {
    sd_card_status = 1;   // SD card in the slot
  }
}

const char* readIDFromSDCard() {
  setupSD();
  if(sd_card_status == 1){
    File configFile = SD.open("sys/wioID.json", "r");
    if (!configFile) {
      Serial.println("Failed to open config file!");
      return "";
    }

    String buf = configFile.readString();

    // Parse the JSON data using ArduinoJson
    StaticJsonDocument<200> doc; // Adjust the buffer size based on your YAML file's size
    DeserializationError error = deserializeJson(doc, buf);

    if (error) {
      Serial.println("Failed to parse JSON file!");
      Serial.println(error.c_str());
      return "";
    }

    // Access the data from the JSON object (assuming it's successfully parsed)
    const char* id = doc["id"];

    configFile.close();
    return id;
  }else{
    return "";
  }
}
