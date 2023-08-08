/**
 * @file networkConnection.cpp
 * @author Fabian Reifler
 * @brief Verbindungsaufbau zum WLAN und MQTT Broker
 * @version 0.1
 * @date 24.04.2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "networkConnection.h"
#include "wio_wifi.h"
#include "wio_mqtt.h"
#include "display.h"

// Module global parameters
// Connection state WLAN and MQTT
static connection_state_t connectionState = {
    .mqtt_status = DISCONNECTED,
    .mqtt_pub_status = false,
    .mqtt_sub_status = false,
    .wlan_status = DISCONNECTED,
    .wlan_strength = 0,
    .wlan_channel = 0};

static char *mqtt_broker;
static uint16_t mqtt_port;

// intervals for periodic tasks
const long scanInterval = 500;            ///< Interval time for WiFi strength und channel scanning
const long interfaceIconIntervall = 1000; ///< Interval time for icons refreshing
const long mqttStateIntervall = 5000;     ///< Interval time for MQTT state

/**
 * @brief In dieser Funktion werden Aufgaben und Funktionen, nach Ablauf eines
 * bestimmten Intervalls, ausgeführt.
 *
 */
void networkConnectionHandler(wio_wifi *wio_Wifi, wio_mqtt *wio_MQTT)
{
  static long previousMillis[] = {0, 0}; // array for the differents previous millis value
                                         // previousMillis[0]: WiFi scan
                                         // previousMillis[1]: update MQTT states
  static int networksFound = -3;         // number of founded networks; -3: start value, -2: scan not triggered, -1: scan not completed
  long currentMillis = millis();         // save millis

  if ((currentMillis - previousMillis[0] >= scanInterval) || previousMillis[0] == 0)
  {
    previousMillis[0] = currentMillis;
    if (wio_Wifi->WiFiStatus() == CONNECTED) // is connected to WLAN?
    {
      connectionState.wlan_status = CONNECTED;
      networksFound = wio_Wifi->getNetworksFound();
      if (networksFound <= -2) // if scan not triggered?
      {
        wio_Wifi->scanNetwork(); // start network scan (asynchronus)
      }

      if (networksFound > 0) // is Scan Done?
      {
        // read No. of current WiFi
        for (int i = 0; i < networksFound; i++)
        {
          // Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : ""); // print information of the selected WiFi
          if (strcmp(wio_Wifi->getScannedSSID(i), wio_Wifi->getSSID()) == 0) // is the selected WiFi the same as the connected WiFi?
          {
            connectionState.wlan_strength = wio_Wifi->measureRSSI(i); // read RSSI
            connectionState.wlan_channel = wio_Wifi->readChannel(i);  // read WiFi channel
            break;
          }
          wio_Wifi->deleteScanResults(); // delete scan results and reset scan progress
        }
      }
    }
    else
    {
      connectionState.wlan_status = DISCONNECTED;
      connectionState.wlan_strength = -99;
      wio_Wifi->reconnect();
    }
  }

  if ((currentMillis - previousMillis[1] >= mqttStateIntervall) || previousMillis[1] == 0)
  {
    if (wio_Wifi->WiFiStatus() == CONNECTED)
    {
      if (wio_MQTT->isConnected()) // check MQTT connection
      {
        connectionState.mqtt_status = CONNECTED;
        if (connectionState.mqtt_status)
        {
          wio_MQTT->clientLoop();             // MQTT client loop
          previousMillis[1] = currentMillis;  // refresh previousMillis
          wio_MQTT->setPublishState(false);   // reset publish state in the mqtt library
          wio_MQTT->setSubscribeState(false); // reset subscribe state in the mqtt library
        }
      }
      else
      {
        connectionState.mqtt_status = DISCONNECTED;
        wio_MQTT->reconnect(mqtt_broker, mqtt_port); // reconnect to MQTT broker
      }
    }
    else
    {
      connectionState.mqtt_status = DISCONNECTED;
    }
  }
}

connection_state_t *getConnectionStatePtr()
{
  return &connectionState;
}

void changeMQTTBroker(const char *broker, uint16_t port, wio_mqtt *wio_MQTT)
{
  mqtt_broker = strdup(broker);
  mqtt_port = port;
  connectionState.mqtt_status = DISCONNECTED;
  wio_MQTT->reconnect(mqtt_broker, mqtt_port);
}

int getMQTTStatus()
{
  return connectionState.mqtt_status;
}

bool getMQTTPubStatus()
{
  return connectionState.mqtt_sub_status;
}

bool getMQTTSubStatus()
{
  return connectionState.mqtt_sub_status;
}

int getWLANStatus()
{
  return connectionState.wlan_status;
}

int getWLANStrength()
{
  return connectionState.wlan_strength;
}

int getWLANChannel()
{
  return connectionState.wlan_channel;
}