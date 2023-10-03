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
#include <PubSubClient.h>
#include <rpcWiFi.h>
#include "wio_mqtt.h"
#include <utils.h>
#include <stdarg.h>

/********************************************************************************************
*** Objects
********************************************************************************************/
WiFiClient wioWiFiClient;
PubSubClient wioMqttClient(wioWiFiClient);
typedef void (*cbLog_)(const char *s, bool b);
static cbLog_ PrintToLog;

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
  PrintToLog = func;
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
void wio_mqtt::initMQTT(message_cb func, const char *mqtt_user, const char *mqtt_password, const char *wioTerminalID)
{
  _callback = func;                       // save Callback function
  PrintToLog("Start MQTT Init", false); // write to the log
  Serial.println("Start MQTT Init");
  
  setCredentials(mqtt_user, mqtt_password);

  // Create a wioMqttClient ID
  m_clientId = "dollhouseWioTerminal-"; // generate id
  m_clientId += String(wioTerminalID);

  snprintf(m_log_text, BUFFER_LENGTH, "- ID: %s", m_clientId.c_str()); // write to the log
  m_log_text[BUFFER_LENGTH - 1] = '\0';
  PrintToLog(m_log_text, false);
  Serial.println(m_clientId);

  wioMqttClient.setCallback(_callback);
}

/**
 * @brief Sets the credentials to authenticate with
 * 
 * @param user  Username to authenticate with
 * @param pass  Password to authenticate with
 * 
 * @note The credentials are applied with the next reconnect.
*/
void wio_mqtt::setCredentials(const char *user, const char *pass) {
  m_user = user;
  m_pass = pass;
}

/**
 * @brief Diese Methode publiziert eine MQTT Nachricht. Char-Array als Payload.
 *
 * @param topic Topic (Name) der Nachricht
 * @param payload Payload (Nutzdaten/Inhalt) der Nachricht
 * @param retain Wenn true: Speichert die Nachricht auf dem Broker
 */
void wio_mqtt::publishTopic(const char *topic, const char *payload, bool retain)
{
  const size_t topic_len = strlen(topic);
  size_t padding = TOPIC_LENGTH - topic_len;

  Serial.print("PUBLISH "); // print infos to SerialPort
  Serial.print(topic);
  while (padding-- > 0) {
    Serial.print(" ");
  }
  Serial.print(" =\t");
  Serial.println(payload);

  m_publishedSomething = true;

  wioMqttClient.publish(topic, payload, retain);
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
  publishTopicPrintf(topic, retain, "%d", payload);
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
  publishTopicPrintf(topic, retain, "%f", payload);
}

void wio_mqtt::publishTopicPrintf(const char *topic, bool retain, const char *format, ...) {
  char buffer[BUFFER_LENGTH];
  
  va_list args;
  va_start(args, format);

  int ret = vsnprintf(buffer, BUFFER_LENGTH, format, args);
  if (ret >= BUFFER_LENGTH) {
    buffer[BUFFER_LENGTH - 1] = '\0';
  }

  va_end(args);

  publishTopic(topic, buffer, retain);
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
  /*
  Serial.println("+++ Subscribe List +++"); // print topics in list to SerialPort
  Serial.print(len);
  Serial.print(" - Liste: ");
  Serial.println(list);
  */

  unsigned int topic_cnt = len / TOPIC_LENGTH; // calc number of topics
  char topic[TOPIC_LENGTH];

  Serial.println("Subscribing for:");
  for (unsigned int i = 0; i < topic_cnt; i++) // iterate through Subscribe-List
  {
    strncpy(topic, (const char *)list, TOPIC_LENGTH);
    topic[TOPIC_LENGTH - 1] = '\0';

    Serial.print("\t");
    Serial.print(topic);
    Serial.print(" ... ");

    wioMqttClient.subscribe(topic); // subscribe topic

    Serial.println("done");

    list += TOPIC_LENGTH; // increment to the next topic
  }
  Serial.println("Subscription done");
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
void wio_mqtt::reconnect(const char *mqtt_broker, uint16_t mqtt_port)
{
  snprintf_safe(m_log_text, BUFFER_LENGTH, "- Connecting to %s", mqtt_broker);
  PrintToLog(m_log_text, false);
  Serial.println(m_log_text);

  if (wioMqttClient.connected()) {
    Serial.println("Disconnecting current link");
    wioMqttClient.disconnect();
  }

  wioMqttClient.setServer(mqtt_broker, mqtt_port);
  if (wioMqttClient.connect(m_clientId.c_str(), m_user.c_str(), m_pass.c_str())) {
    PrintToLog("- Connected", false); // write to the log
    Serial.println("Connected");
    subscribeList(ptr_topicList, topicListLen);    // subscribe the topic list
    PrintToLog("- Subscribed to topics", false); // write to the log
    Serial.println("Subscribed to topics");
  }
  else
  {
    Serial.println("failed, rc=");
    Serial.print(wioMqttClient.state());
  }
}

/**
 * @brief Diese Methode verarbeitet eingehende und ausgehende Nachrichten und frischt die Verbindung auf.
 * Damit die MQTT Funktionen ordentlich funktionieren, muss diese Methode periodisch aufgerufen werden.
 *
 */
void wio_mqtt::loop()
{
  wioMqttClient.loop();
}

/**
 * @brief Diese Methode gibt den Publish Status zurück.
 *
 * @return true Es wurde kürzlich ein Topic publiziert
 * @return false Es wurde nichts publiziert
 */
bool wio_mqtt::getPublishState()
{
  return m_publishedSomething;
}

/**
 * @brief Diese Methode setzt den Publish Status zurück
 */
void wio_mqtt::resetPublishState(void)
{
  m_publishedSomething = false;
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
 * @brief Diese Methode setzt den Subscribe Status
 *
 * @param state Wert für den Subscribe Status
 */
void wio_mqtt::setSubscribeState(bool state)
{
  subState = state;
}