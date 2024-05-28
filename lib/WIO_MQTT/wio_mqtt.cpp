#include "wio_mqtt.h"

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
#include <utils.h>
#include <stdarg.h>

/********************************************************************************************
*** Constructor
********************************************************************************************/

/**
 * @brief Construct a new wio mqtt::wio mqtt object
 *
 * @param func Funktionsadresse der Funktion für das Hinzufügen des Log Text
 */
wio_mqtt::wio_mqtt(log_cb logger) : m_logger(logger), m_messageHandler(NULL), m_client(), m_mqtt(m_client)
{
  randomSeed(analogRead(0));
}

/********************************************************************************************
*** Public Methods
********************************************************************************************/
/**
 * @brief Diese Methode setzt die MQTT-Parameter und stellt eine Verbindung zum MQTT Broker her.
 *
 * @param messageHandler Funktionsadresse der Callback Funktion für die Auswertung der abbonierten Topics
 */
void wio_mqtt::initMQTT(message_cb messageHandler, const char *mqtt_user, const char *mqtt_password, const char *wioTerminalID)
{
  LogAdd("Start MQTT Init", false); // write to the log
  m_messageHandler = messageHandler;
  
  setCredentials(mqtt_user, mqtt_password);

  // Create a wioMqttClient ID
  m_clientId = "dollhouseWioTerminal-"; // generate id
  m_clientId += String(wioTerminalID);

  snprintf(m_log_text, BUFFER_LENGTH, "- ID: %s", m_clientId.c_str()); // write to the log
  m_log_text[BUFFER_LENGTH - 1] = '\0';

  LogAdd(m_log_text, false);
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

  m_mqtt.publish(topic, payload, retain);
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
  m_mqtt.subscribe(topic); // subscribe the topic
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

    m_mqtt.subscribe(topic); // subscribe topic

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
  return m_mqtt.connected();
}

/**
 * @brief Diese Methode stellt die Verbindung zum Broker wieder her und abboniert die Subscribe-Liste.
 *
 */
void wio_mqtt::reconnect(const char *mqtt_broker, uint16_t mqtt_port)
{
  m_broker = mqtt_broker;
  m_port = mqtt_port;

  snprintf_safe(m_log_text, BUFFER_LENGTH, "- Connecting to %s", mqtt_broker);
  LogAdd(m_log_text, false);

  if (isConnected()) {
    Serial.println("Disconnecting current link");
    // m_mqtt.loop(); behebt Absturzproblem :disconnect() nicht stimmt. Dies ist ein Workaround, um ein Bug im WiFiClient zu umgehen.
    // Kern des Problems ist WiFiClient::flush(). Die Funktion recv wird vermutlich fälschlicherweise benutzt. Eigenltich müsste der rx- Buffer geleert werden.
    m_mqtt.loop();
    m_mqtt.disconnect();
  }

  Serial.printf("Reconnect - Set broker to %s:%u\n", m_broker.c_str(), m_port);
  m_mqtt.setServer(m_broker.c_str(), m_port);
  m_mqtt.setCallback(m_messageHandler);

  Serial.println("Reconnect - Connect");
  if (m_mqtt.connect(m_clientId.c_str(), m_user.c_str(), m_pass.c_str())) {
    LogAdd("- Connected", false); // write to the log
    subscribeList(ptr_topicList, topicListLen);    // subscribe the topic list
    LogAdd("- Subscribed to topics", false); // write to the log
  }
  else
  {
    Serial.println("failed, rc=");
    Serial.print(m_mqtt.state());
  }
}

/**
 * @brief Diese Methode verarbeitet eingehende und ausgehende Nachrichten und frischt die Verbindung auf.
 * Damit die MQTT Funktionen ordentlich funktionieren, muss diese Methode periodisch aufgerufen werden.
 *
 */
void wio_mqtt::loop()
{
  m_mqtt.loop();
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

void wio_mqtt::LogAdd(const char *str, bool append) {
  if (m_logger) {
    m_logger(str, append);
  }
  
  Serial.print(str);
  if (!append) {
    Serial.println();
  }
}
