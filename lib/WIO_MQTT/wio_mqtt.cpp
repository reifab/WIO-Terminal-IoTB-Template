/**
 * @file wio_mqtt.cpp
 * @author Beat Sturzenegger / Fabian Reifler
 * @brief IoTB MQTT Bibliothek für das WIO Terminal
 * @version 1.5
 * @date 08.03.2023
 *
 * @copyright Copyright (c) 2023
 *
 */

/********************************************************************************************
*** Includes
********************************************************************************************/
#include <ArduinoMqttClient.h>
#include <rpcWiFi.h>
#include "wio_mqtt.h"

/********************************************************************************************
*** Objects
********************************************************************************************/
WiFiClient wioWiFiClient;
MqttClient wioMqttClient(wioWiFiClient);
typedef void (*cbLog_)(const char *s, bool b);
static cbLog_ cbMQTTLog;

/********************************************************************************************
*** Constructor
********************************************************************************************/

/**
 * @brief Construct a new wio mqtt::wio mqtt object
 *
 * @param func Funktionsadresse der Funktion für das Hinzufügen des Log Text
 */
wio_mqtt::wio_mqtt(void (&func)(const char *, bool b))
{
  cbMQTTLog = func;
  randomSeed(analogRead(0));
}

/********************************************************************************************
*** Public Methods
********************************************************************************************/
/**
 * @brief Diese Methode setzt die MQTT-Parameter und stellt eine Verbindung zum MQTT Broker her.
 *
 * @param func Funktionsadresse der Callback Funktion für die Auswertung der abbonierten Topics
 */
void wio_mqtt::initMQTT(void (&func)(int))
{
  _callback = func;                                    // save Callback function
  (*cbMQTTLog)("Start MQTT Init", false);              // write to the log
  sprintf(logText, "- Connecting to %s", default_mqtt_broker); // write to the log
  (*cbMQTTLog)(logText, false);

  Serial.print("Attempting MQTT connection...");
  // Create a random wioMqttClient ID
  String clientId = "WioTerminal"; // generate id
  clientId += String(random(0xffff), HEX);
  Serial.print(" - ");
  Serial.print(clientId);
  Serial.print(" -...");
  sprintf(logText, "- ID: %s", clientId.c_str()); // write to the log
  (*cbMQTTLog)(logText, false);

  wioMqttClient.setId(clientId);
  wioMqttClient.setUsernamePassword(mqtt_user, mqtt_password);
  wioMqttClient.onMessage(_callback);         // set callback function
  subscribeList(ptr_topicList, topicListLen); // subscribe the topic list
}

/**
 * @brief Diese Methode publiziert eine MQTT Nachricht. Char-Array als Payload.
 *
 * @param topic Topic (Name) der Nachricht
 * @param payload Payload (Nutzdaten/Inhalt) der Nachricht
 * @param retain Wenn true: Speichert die Nachricht auf dem Broker
 */
void wio_mqtt::publishTopic(const char *topic, char *payload, bool retain)
{
  pubState = true;           // set publish state to TRUE, because somthing will be sended
  Serial.println("PUBLISH"); // print infos to SerialPort
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(payload);

  // publish the message:
  wioMqttClient.beginMessage(topic, retain);
  wioMqttClient.print(payload);
  wioMqttClient.endMessage();
}

/**
 * @brief Diese Methode publiziert eine MQTT Nachricht. Konstanter char-Array als Payload.
 *
 * @param topic Topic (Name) der Nachricht
 * @param payload Payload (Nutzdaten/Inhalt) der Nachricht
 * @param retain Wenn true: Speichert die Nachricht auf dem Broker
 */
void wio_mqtt::publishTopic(const char *topic, const char *payload, bool retain)
{
  char payloadCopy[strlen(payload) + 1];
  strcpy(payloadCopy, payload);
  wio_mqtt::publishTopic(topic, payloadCopy, retain);
}

/**
 * @brief Diese Methode publiziert eine MQTT Nachricht. Integer als Payload.
 *
 * @param topic Topic (Name) der Nachricht
 * @param payload Payload (Nutzdaten/Inhalt) der Nachricht
 * @param retain Wenn true: Speichert die Nachricht auf dem Broker
 */
void wio_mqtt::publishTopic(const char *topic, int payload, bool retain)
{
  pubState = true;           // set publish state to TRUE, because somthing will be sended
  Serial.println("PUBLISH"); // print infos to SerialPort
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(payload);

  // publish the message:
  wioMqttClient.beginMessage(topic, retain);
  wioMqttClient.print(payload);
  wioMqttClient.endMessage();
}

/**
 * @brief Diese Methode publiziert eine MQTT Nachricht. Fliesskommazahl (Float) als Payload. Der Payload wird auf 3 Nachkommastellen gekürzt
 *
 * @param topic Topic (Name) der Nachricht
 * @param payload Payload (Nutzdaten/Inhalt) der Nachricht
 * @param retain Wenn true: Speichert die Nachricht auf dem Broker
 */
void wio_mqtt::publishTopic(const char *topic, float payload, bool retain)
{
  pubState = true;           // set publish state to TRUE, because somthing will be sended
  Serial.println("PUBLISH"); // print infos to SerialPort
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(payload);

  // publish the message:
  wioMqttClient.beginMessage(topic, retain);
  wioMqttClient.print(payload, 3);
  wioMqttClient.endMessage();
}

/**
 * @brief Diese Methode abboniert ein Topic
 *
 * @param topic Topicname (Name der zu abbonierenden Nachicht)
 */
void wio_mqtt::subscribeTopic(char *topic)
{
  wioMqttClient.subscribe(topic); // subscribe the topic
}

/**
 * @brief Diese Methode speichert Informationen zu der Subscribe-Liste.
 *
 * @param list Adresse der Subscribe-Liste
 * @param len Länge der Liste (Anzahl Zeichen --> Anzahl Topics x TOPIC_LENGTH)
 */
void wio_mqtt::addSubscribeList(char *list, unsigned int len)
{
  ptr_topicList = list; // save list address
  topicListLen = len;   // save list length
}

/**
 * @brief Diese Methode abboniert die Subscribe-Liste.
 *
 * @param list Adresse der Subscribe-Liste
 * @param len Länge der Subscribe-Liste
 */
void wio_mqtt::subscribeList(char *list, unsigned int len)
{
  Serial.println("+++ Subscribe List +++"); // print topics in list to SerialPort
  Serial.print(len);
  Serial.print(" - Liste: ");
  Serial.println(list);

  unsigned int topic_cnt = len / TOPIC_LENGTH; // calc number of topics
  char topic[TOPIC_LENGTH];

  for (unsigned int i = 0; i < topic_cnt; i++) // iterate through Subscribe-List
  {
    strcpy(topic, (const char *)list);
    wioMqttClient.subscribe(topic); // subscribe topic
    Serial.print("Topic ");         // print topics in list to SerialPort
    Serial.print(i);
    Serial.print(": ");
    Serial.println(topic);
    for (int ii = 0; ii < TOPIC_LENGTH; ii++) // increment to the next topic
    {
      list++;
    }
  }
}

/**
 * @brief Diese Methode überpüft, ob die Verbindung zum MQTT noch besteht.
 *
 * @return true Es besteht @b eine Verbindung zum MQTT Broker
 * @return false Es besteht @b keine Verbindung zum MQTT Broker
 */
bool wio_mqtt::isConnected()
{
  return wioMqttClient.connected(); // read connection state
}

/**
 * @brief Diese Methode stellt die Verbindung zum Broker wieder her.
 *
 */
void wio_mqtt::reconnect()
{
  // Attempt to connect
  if (wioMqttClient.connect(default_mqtt_broker, default_mqtt_port))
  {
    (*cbMQTTLog)("- Connected", false); // write to the log
    Serial.println("connected");
    subscribeList(ptr_topicList, topicListLen);    // subscribe the topic list
    (*cbMQTTLog)("- Subscribed to Topics", false); // write to the log
  }
  else
  {
    Serial.print("failed, rc=");
    Serial.print(wioMqttClient.connectError());
  }
}

/**
 * @brief Diese Methode verarbeitet eingehende und ausgehende Nachrichten und frischt die Verbindung auf.
 * Damit die MQTT Funktionen ordentlich funktionieren, muss diese Methode periodisch aufgerufen werden.
 *
 */
void wio_mqtt::clientLoop()
{
  wioMqttClient.poll();
}

/**
 * @brief Diese Methode gibt den Publish Status zurück.
 *
 * @return true Es wurde kürzlich ein Topic publiziert
 * @return false Es wurde nichts publiziert
 */
bool wio_mqtt::getPublishState()
{
  return pubState;
}

/**
 * @brief Diese Methode gibt den Subscribe Status zurück.
 *
 * @return true Es wurde kürzlich ein Topic empfangen
 * @return false Es wurde kein abboniertes Topicaktualisiert
 */
bool wio_mqtt::getSubscribeState()
{
  return subState;
}

/**
 * @brief Diese Methode liesst einen empfangenen Topic zurück
 *
 * @return Zeiger auf ein char- Array, welches das Topic enthält
 */
const char *wio_mqtt::getMessageTopic(void)
{
  String msgTopicStr = wioMqttClient.messageTopic();
  char *msgTopicCharArray = new char[msgTopicStr.length() + 1];
  msgTopicStr.toCharArray(msgTopicCharArray, msgTopicStr.length() + 1);
  return msgTopicCharArray;
}

/**
 * @brief Diese Methode liesst die empfangenen Nutzdaten zurück
 *
 * @return Zeiger auf ein char- Array, welches den Payload enthält
 */
const char *wio_mqtt::getMessagePayload(void)
{
  String msgPayloadStr = wioMqttClient.readString();
  char *msgPayloadCharArray = new char[msgPayloadStr.length() + 1];
  msgPayloadStr.toCharArray(msgPayloadCharArray, msgPayloadStr.length() + 1);
  return msgPayloadCharArray;
}

/**
 * @brief Diese Methode setzt den Publish Status
 *
 * @param state Wert für den Publish Status
 */
void wio_mqtt::setPublishState(bool state)
{
  pubState = state;
}

/**
 * @brief Diese Methode setzt den Subscribe Status
 *
 * @param state Wert für den Subscribe Status
 */
void wio_mqtt::setSubscribeState(bool state)
{
  subState = state;
}