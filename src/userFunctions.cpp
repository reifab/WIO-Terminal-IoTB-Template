/**
 * @file userFunctions.cpp
 * @author Fabian Reifler
 * @brief Funktionen vom Anwender
 * Du kannst deinen Code zwischen <tt>"START USER CODE"</tt> und <tt>"END USER CODE"</tt> einfügen
 * @version 0.1
 * @date 17.05.2023
 *
 * @copyright Copyright (c) 2023
 */

/********************************************************************************************
*** Includes
********************************************************************************************/
#include <Arduino.h>
#include "userFunctions.h"
#include "pages.h"
#include "wio_mqtt.h"
#include "display.h"
#include "Wire.h"
#include "Adafruit_BME680.h"
#include "ArduinoJson.h"
#include "networkConnection.h"
#include "readConfigSDCard.h"

// START USER CODE: Includes

// END USER CODE: Includes

/********************************************************************************************
*** Defines
********************************************************************************************/

// START USER CODE: Defines
#define I2C_ADDRESS_BME680 0x76
#define HEATER_DIGITAL_OUT D0

#define ANALOG_IN_SOLAR A0
#define DIGITAL_IN_WINDOW D2
// END USER CODE: Defines

/********************************************************************************************
*** Functionprototypes
********************************************************************************************/
void onMqttMessage(int messageSize);
// START USER CODE: Functionprototypes
void setHeaterOn(void);
void setHeaterOff(void);
// END USER CODE: Functionprototypes

/********************************************************************************************
*** Module Global Parameters
********************************************************************************************/
// MQTT parameters
char topicList[][TOPIC_LENGTH] = ///< Liste der MQTT Topics, die abboniert werden sollen. Die Topics werden als String eingegeben. Das Letzte Zeichen sollte ein '\0' sein.
                                 ///< Die einzelnen Strings werden mit einem Komma getrennt (Siehe Beispiel. Coming soon...)
                                 ///< @attention Maximale 50 Zeichen pro Topic!
    {
        // START USER CODE: Subscribed Topics
        "reserved\0", // Reserviert, darf nicht benutzt werden
        "meinHaus/1OG/Heizungmein\0"
        // END USER CODE: Subscribed Topics
};

static bool changeSettings = false; ///< Flag, um den Broker zu wechseln. Wird in der Funktion onMqttMessage() gesetzt.
char *brokerToConnect;              ///< Broker, zu dem gewechselt werden soll. Wird in der Funktion onMqttMessage() gesetzt.
char *functionality;                ///< Funktionalität, die der Wio Terminal übernehmen soll. Wird in der Funktion onMqttMessage() gesetzt.
uint16_t portToConnect;             ///< Port, zu dem gewechselt werden soll. Wird in der Funktion onMqttMessage() gesetzt.

// START USER CODE: Global Variables
float g_temperature;
float g_pressure;
float g_humidity;

char g_heater[4] = "off";

int g_solarVoltage;
char g_window_state[10];
// END USER CODE: Global Variables

/********************************************************************************************
*** Objects
********************************************************************************************/
static wio_mqtt *wio_MQTT;
// START USER CODE: Objects
Adafruit_BME680 bme;
// END USER CODE: Objects

int initUserFunctions(wio_mqtt *ptr_wio_MQTT)
{
  // START USER CODE: Setup
  int currentPage = 1; // set currentPage

  // END USER CODE: Setup

  // MQTT Configuration
  readConfigFromSDCard();
  wio_MQTT = ptr_wio_MQTT;
  String configTopic = "wioTerminals/configTerminal-";
  configTopic += readIDOfWioTerminal();

  strcpy(topicList[0], configTopic.c_str());
  wio_MQTT->addSubscribeList(topicList[0], sizeof(topicList)); // subscribe to all topics in topicList
  wio_MQTT->initMQTT(onMqttMessage);                           // init MQTT with callback function

  return currentPage;
}

int userFunctionsHandler(int currentPage, wio_mqtt *wio_MQTT, page_t pages_array[])
{
  const long userDefinedIntervall = 2000; // Interval time for user defined perdiodic task
  static long previousMillis;             // for user
  long currentMillis = millis();          // save millis

  // START USER CODE: variables

  // END USER CODE: variables

  //================================//
  // Page dependet tasks            //
  //================================//
  switch (currentPage)
  {
  case 0:
    // START USER CODE: Specific Page 0 Code

    // END USER CODE: Specific Page 0 Code
    break;

  case 1:
    // START USER CODE: Specific Page 1 Code
    if ((currentMillis - previousMillis >= userDefinedIntervall) || previousMillis == 0)
    {
      previousMillis = currentMillis; // refresh previousMillis
      // START USER CODE: user periodic task
      bme.performReading();
      Serial.println(bme.temperature);
      Serial.println(bme.pressure);
      Serial.println(bme.humidity);

      g_temperature = bme.temperature;
      g_pressure = bme.pressure / 100; // to hPa
      g_humidity = bme.humidity;

      wio_MQTT->publishTopic("meinHaus/1OG/Temperatur", bme.temperature, false);
      wio_MQTT->publishTopic("meinHaus/1OG/Luftdruck", (float)bme.pressure, false);
      wio_MQTT->publishTopic("meinHaus/1OG/Luftfeuchtigkeit", bme.humidity, false);
      wio_MQTT->publishTopic("meinHaus/1OG/HeizungStatus", g_heater, false);

      // END USER CODE: user periodic task
    }

      if (pages_array[currentPage].lines[0].value != g_temperature)
      {
          pages_array[currentPage].lines[0].value = g_temperature;
          updateLine(currentPage, 0, ONLY_VALUE);
      }

      if (pages_array[currentPage].lines[1].value != g_humidity)
      {
          pages_array[currentPage].lines[1].value = g_humidity;
          updateLine(currentPage, 1, ONLY_VALUE);
      }

      if (pages_array[currentPage].lines[2].value != g_pressure)
      {
          pages_array[currentPage].lines[2].value = g_pressure;
          updateLine(currentPage, 2, ONLY_VALUE);
      }

      if (strcmp(pages_array[currentPage].lines[3].text, g_heater) != 0)
      {
          strcpy(pages_array[currentPage].lines[3].text, g_heater);
          updateLine(currentPage, 3, ONLY_VALUE);
      }

    // END USER CODE: Specific Page 1 Code
    break;

  case 2:
    // START USER CODE: Specific Page 2 Code
    const uint8_t firstLine = 0;
    const uint8_t secondLine = 1;

    if ((currentMillis - previousMillis >= userDefinedIntervall) || previousMillis == 0)
    {
      previousMillis = currentMillis; // refresh previousMillis
                                      // START USER CODE: user periodic task

      g_solarVoltage = analogRead(ANALOG_IN_SOLAR) * 100 / 1024;
      if (digitalRead(DIGITAL_IN_WINDOW))
      {
        strcpy(g_window_state, "zu");
      }
      else
      {
        strcpy(g_window_state, "offen");
      }

      wio_MQTT->publishTopic("meinHaus/Dach/Solarzelle/Leistung", g_solarVoltage, 0);
      wio_MQTT->publishTopic("meinHaus/2OG/FensterStatus", g_window_state, 0);

    }
    if (pages_array[currentPage].lines[firstLine].value != g_solarVoltage)
    {
      pages_array[currentPage].lines[firstLine].value = g_solarVoltage;
      updateLine(currentPage,firstLine,ONLY_VALUE);
    }

    if (strcmp(pages_array[currentPage].lines[secondLine].text, g_window_state) != 0)
    {
      strcpy(pages_array[currentPage].lines[secondLine].text, g_window_state);
      updateLine(currentPage, secondLine, ONLY_VALUE);
    }
    // END USER CODE: Specific Page 1 Code
    break;
    // START USER CODE: more Page specific Code

    // END USER CODE: more Page specific Code
  }

  if (changeSettings)
  {
    changeMQTTBroker(brokerToConnect, portToConnect, wio_MQTT);

    if (strcmp(functionality, "heatingControl") == 0)
    {
      currentPage = 1;
    }

    if (strcmp(functionality, "solar") == 0)
    {
      currentPage = 2;
    }
    drawPage(pages_array, currentPage);
    changeSettings = false;
    Serial.println("Broker changed");
  }
  return currentPage;
}

/********************************************************************************************
*** Functions
********************************************************************************************/

/**
 * @brief MQTT Callback Function for subscibed topics.
 * Diese Funktion wird automatisch aufgerufen, wenn ein abboniertes MQTT Topic eintrifft.\n
 * Im String <tt>@b payload_str</tt> steht der Inhalt des Topics.
 *
 *
 * @param messageSize  Länge der Nachricht
 */
void onMqttMessage(int messageSize)
{
  int elem = 0;
  wio_MQTT->setSubscribeState(true); // set subscribe state (for display)
  const char *topic = wio_MQTT->getMessageTopic();
  const char *payload = wio_MQTT->getMessagePayload();
  StaticJsonDocument<200> jsonDocConfig;

  Serial.println("Message received:"); // print infos to SerialPort
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(payload);

  while (strcmp(topic, topicList[elem])) // search topic in topicList
  {
    elem++; // if not found, increment elem
  }

  switch (elem)
  {
  case 0:
    // START USER CODE: first element of the topicList (wioTerminal/config)l
    deserializeJson(jsonDocConfig, payload);
    brokerToConnect = strdup(jsonDocConfig["brokerToConnect"]);
    portToConnect = jsonDocConfig["portToConnect"];
    functionality = strdup(jsonDocConfig["functionality"]);
    changeSettings = true;

    // END USER CODE: first element of the topicList
    break;
  case 1:
    // START USER CODE: second element of the topicList
    Serial.println(payload); // Beispiel
    if (strcmp(payload, "on") == 0)
    {
      setHeaterOn();
    }
    if (strcmp(payload, "off") == 0)
    {
      setHeaterOff();
    }

    if (strcmp(payload, "1") == 0)
    {
      setHeaterOn();
    }
    if (strcmp(payload, "0") == 0)
    {
      setHeaterOff();
    }
    // END USER CODE: second element of the topicList
    break;

    // START USER CODE: more elements of the topicList

    // END USER CODE: more elements of the topicList
  }
}

// START USER CODE: user functions
void setHeaterOn(void)
{
  digitalWrite(HEATER_DIGITAL_OUT, HIGH);
  strcpy(g_heater, "on");
}
void setHeaterOff(void)
{
  digitalWrite(HEATER_DIGITAL_OUT, LOW);
  strcpy(g_heater, "off");
}
// END USER CODE: user functions
