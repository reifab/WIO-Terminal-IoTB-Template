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
#include <utils.h>
#include <stdlib.h>
#include <string.h>

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
#include "Servo.h"
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
static void jsonValueFetch(char **dst, StaticJsonDocument<500> &doc, const char *key);
static void onMqttMessage(char *topic, byte *payloadbytes, unsigned int len);
static void prepareTopicNames(void);
static void replaceTopicName(char **name, const char *topic);

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
        "reserved\0", // Reserviert für ein Konfigurationstopic
        "reserved\0", // Reserviert für XXX/1OG/Heizung, wobei XXX ein Prefix (meinHaus oder HausX) ist
        "Haus1/2OG/WinkelServo\0"
        // END USER CODE: Subscribed Topics
};

static bool changeSetup = false;
static char *brokerToConnect;  ///< Broker, zu dem gewechselt werden soll. Wird in der Funktion onMqttMessage() gesetzt.
static char *functionality;    ///< Funktionalität, die der Wio Terminal übernehmen soll. Wird in der Funktion onMqttMessage() gesetzt.
static uint16_t portToConnect; ///< Port, zu dem gewechselt werden soll. Wird in der Funktion onMqttMessage() gesetzt.

static char *mqtt_user;     ///< MQTT Benutzername.
static char *mqtt_password; ///< MQTT Passwort.
static char *mqtt_prefix;   ///< MQTT Prefix, der vor alle Topics gesetzt wird. Wird in der Funktion onMqttMessage() gesetzt.

static char *topic_floor1_temp_name = NULL;
static char *topic_floor1_pressure_name = NULL;
static char *topic_floor1_humidity_name = NULL;
static char *topic_floor1_heating_state_name = NULL;
static char *topic_floor1_heating_control_name = NULL;
static char *topic_floor2_windowstate_name = NULL;
static char *topic_roof_pv_power_name = NULL;

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
static wio_mqtt *wio_MQTT = NULL;
// START USER CODE: Objects
Adafruit_BME680 bme;
Servo myservo;
// END USER CODE: Objects

void setMQTTUserAndPassword(const char *mqtt_user, const char *mqtt_password)
{
  wio_MQTT->initMQTT(onMqttMessage, mqtt_user, mqtt_password, getWioTerminalID());
}

int initUserFunctions(wio_mqtt *ptr_wio_MQTT)
{
  // START USER CODE: Setup
  if (mqtt_prefix != NULL)
  {
    free(mqtt_prefix);
  }
  mqtt_prefix = strdup("meinHaus/");

  int currentPage;
  if (strcmp(getWioTerminalFuntionality(), "heatingControl") == 0)
  {
    currentPage = 1;
    if (!bme.begin(I2C_ADDRESS_BME680))
    {
      Serial.println("Could not find a valid BME680 sensor, check wiring!");
    }
  }
  else if (strcmp(getWioTerminalFuntionality(), "solar") == 0)
  {
    currentPage = 2;
  }
  else if (strcmp(getWioTerminalFuntionality(), "fenster") == 0)
  {
    myservo.attach(D0);
    currentPage = 3;
  }
  else
  {
    currentPage = 0;
  }

  prepareTopicNames();

  // END USER CODE: Setup

  // MQTT Configuration
  wio_MQTT = ptr_wio_MQTT;
  String configTopic = "setup/wioTerminals/configTerminal-";
  configTopic += getWioTerminalID();

  strncpy(topicList[0], configTopic.c_str(), TOPIC_LENGTH);
  wio_MQTT->addSubscribeList(topicList[0], sizeof(topicList)); // subscribe to all topics in topicList

  return currentPage;
}

static void prepareTopicNames(void)
{
  replaceTopicName(&topic_floor1_temp_name, "1OG/Temperatur");
  replaceTopicName(&topic_floor1_pressure_name, "1OG/Luftdruck");
  replaceTopicName(&topic_floor1_humidity_name, "1OG/Luftfeuchtigkeit");
  replaceTopicName(&topic_floor1_heating_state_name, "1OG/HeizungStatus");
  replaceTopicName(&topic_floor2_windowstate_name, "2OG/FensterStatus");
  replaceTopicName(&topic_roof_pv_power_name, "Dach/Solarzelle/Leistung");
  replaceTopicName(&topic_floor1_heating_control_name, "1OG/Heizung\0");
}

static void replaceTopicName(char **name, const char *topic)
{
  if (*name != NULL)
  {
    free(*name);
    *name = NULL;
  }
  *name = strcattodup(mqtt_prefix, topic);
}

int userFunctionsHandler(int currentPage, wio_mqtt *wio_MQTT, page_t pages_array[])
{
  const long userDefinedIntervall = 1000; // Interval time for user defined perdiodic task
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

      g_temperature = bme.temperature;
      g_pressure = bme.pressure / 100; // to hPa
      g_humidity = bme.humidity;

      wio_MQTT->publishTopic(topic_floor1_temp_name, bme.temperature, false);
      wio_MQTT->publishTopic(topic_floor1_pressure_name, (float)bme.pressure, false);
      wio_MQTT->publishTopic(topic_floor1_humidity_name, bme.humidity, false);
      wio_MQTT->publishTopic(topic_floor1_heating_state_name, g_heater, false);
      // Serial.println("User Function Handler: Messages published");
      //  END USER CODE: user periodic task
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

      wio_MQTT->publishTopic(topic_roof_pv_power_name, g_solarVoltage, false);
      wio_MQTT->publishTopic(topic_floor2_windowstate_name, g_window_state, false);
    }
    if (pages_array[currentPage].lines[firstLine].value != g_solarVoltage)
    {
      pages_array[currentPage].lines[firstLine].value = g_solarVoltage;
      updateLine(currentPage, firstLine, ONLY_VALUE);
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

  if (changeSetup)
  {
    if (strcmp(functionality, "heatingControl") == 0)
    {
      currentPage = 1;
      pinMode(HEATER_DIGITAL_OUT, OUTPUT);
      setHeaterOff();
      strncpy(topicList[1], topic_floor1_heating_control_name, TOPIC_LENGTH); // Neues Topic wird danach abboniert zur Heizungssteuerung
    }

    if (strcmp(functionality, "solar") == 0)
    {
      currentPage = 2;
      pinMode(ANALOG_IN_SOLAR, INPUT);
      pinMode(DIGITAL_IN_WINDOW, INPUT);
    }

    if (strcmp(functionality, "fenster") == 0)
    {
      currentPage = 3;
      myservo.attach(D0);
    }

    Serial.println("Processing broker reconnect");
    changeMQTTBroker(brokerToConnect, portToConnect, mqtt_user, mqtt_password, wio_MQTT);
    changeSetup = false;

    drawPage(pages_array, currentPage);

    Serial.println("Processing broker reconnect ended");
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
static StaticJsonDocument<500> jsonDocConfig;
static void onMqttMessage(char *topic, byte *payloadbytes, unsigned int len)
{
  char *payload = (char *)malloc(len + 1);

  memcpy(payload, payloadbytes, len);
  payload[len] = '\0';

  int elem = 0;
  wio_MQTT->setSubscribeState(true); // set subscribe state (for display)

#if BROKER_SETUP_ON_SERIAL
  Serial.printf("Message received: %s\t", topic);
  Serial.println(payload);
#endif

  while (strcmp(topic, topicList[elem])) // search topic in topicList
  {
    elem++; // if not found, increment elem
  }

  switch (elem)
  {
  case 0:
    // START USER CODE: first element of the topicList (wioTerminal/config)
    // Serial.println("Config received");
    deserializeJson(jsonDocConfig, payload);

    if (strcmp(brokerToConnect, strdup(jsonDocConfig["brokerToConnect"])) != 0)
    {
      free(brokerToConnect);
      brokerToConnect = strdup(jsonDocConfig["brokerToConnect"]);
      portToConnect = jsonDocConfig["portToConnect"];

      jsonValueFetch(&mqtt_prefix, jsonDocConfig, "mqtt_prefix");
      jsonValueFetch(&mqtt_user, jsonDocConfig, "mqtt_bootstrap_broker_user");
      jsonValueFetch(&mqtt_password, jsonDocConfig, "mqtt_bootstrap_broker_password");
      jsonValueFetch(&functionality, jsonDocConfig, "functionality");

      prepareTopicNames();

      Serial.println("\tRequest broker reconnect");
      changeSetup = true;
    }
    // END USER CODE: first element of the topicList
    break;
  case 1:
    // START USER CODE: second element of the topicList
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
  case 2:
    // START USER CODE: third element of the topicList
      myservo.write(atoi(payload));
    // END USER CODE: third element of the topicList
    break;
  }

  free(payload);
  payload = NULL;
  Serial.println("Message processed");
}

static void jsonValueFetch(char **dst, StaticJsonDocument<500> &doc, const char *key)
{
  if (*dst)
  {
    free(*dst);
    *dst = NULL;
  }

  const char *value = doc[key];
  if (value)
  {
    *dst = strdup(value);
  }
  else
  {
    *dst = strdup("");
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
