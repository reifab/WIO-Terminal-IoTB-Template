/**
 * @file wio_mqtt.cpp
 * @author Beat Sturzenegger
 * @brief IoTB MQTT Bibliothek für das WIO Terminal
 * @version 1.4
 * @date 18.01.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/********************************************************************************************
*** Includes
********************************************************************************************/
#include <PubSubClient.h>
#include <rpcWiFi.h>
#include "wio_mqtt.h"

/********************************************************************************************
*** Objects
********************************************************************************************/
WiFiClient wioClient;
PubSubClient client(wioClient);
typedef void (*cbLog_)  (const char *s, bool b);
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
*** Public Methodes
********************************************************************************************/
/**
 * @brief Diese Methode setzt die MQTT-Parameter und stellt eine Verbindung zum MQTT Broker her.
 * 
 * @param func Funktionsadresse der Callback Funktion für die Auswertung der abbonierten Topics
 */
void wio_mqtt::initMQTT(void (&func)(char *, byte *, unsigned int))
{
  _callback = func;                                 // save Callback function
  (*cbMQTTLog)("Start MQTT Init", false);           // write to the log
  sprintf(logText, "- Connecting to %s", mqtt_server);    // write to the log 
  (*cbMQTTLog)(logText, false);
  client.setServer(mqtt_server, 1883);              // set MQTT broker url and port
  client.setCallback(_callback);                    // set callback function
  delay(1000);                                      // 1000ms delay
  while (!client.connected())     // Loop until we're reconnected
  {
    reconnect();                                      // connect to MQTT broker
  }
}

/**
 * @brief Diese Methode publiziert eine MQTT Nachricht. Variabler char-Array als Payload.
 * 
 * @param topic Topic (Name) der Nachricht
 * @param payload Payload (Nutzdaten/Inhalt) der Nachricht
 * @param retain Soll die Nachricht gespeichert werden, auch wenn das Topic niemand abonniert hat.
 */
void wio_mqtt::publishTopic_String(const char *topic, char *payload, bool retain)
{
  pubState = true;                                      // set publish state to TRUE, because somthing will be sended
  Serial.println("PUBLISH");                            // print infos to SerialPort
  Serial.print("Topic: "); Serial.println(topic);
  Serial.print("Payload: "); Serial.println(payload);
  client.publish(topic, payload, retain);                       // publish the message
}

/**
 * @brief Diese Methode publiziert eine MQTT Nachricht. Konstanter char-Array als Payload.
 * 
 * @param topic Topic (Name) der Nachricht
 * @param payload Payload (Nutzdaten/Inhalt) der Nachricht
 * @param retain Soll die Nachricht gespeichert werden, auch wenn das Topic niemand abonniert hat.
 */
void wio_mqtt::publishTopic_ConstString(const char *topic, const char *payload, bool retain)
{
  pubState = true;                                      // set publish state to TRUE, because somthing will be sended
  Serial.println("PUBLISH");                            // print infos to SerialPort
  Serial.print("Topic: "); Serial.println(topic);
  Serial.print("Payload: "); Serial.println(payload);
  client.publish(topic, payload, retain);                       // publish the message
}

/**
 * @brief Diese Methode publiziert eine MQTT Nachricht. Integer als Payload.
 * 
 * @param topic Topic (Name) der Nachricht
 * @param payload Payload (Nutzdaten/Inhalt) der Nachricht
 * @param retain Soll die Nachricht gespeichert werden, auch wenn das Topic niemand abonniert hat.
 */
void wio_mqtt::publishTopic_Integer(const char *topic, int payload, bool retain)
{
  char mqttPayload[20];
  pubState = true;                                      // set publish state to TRUE, because somthing will be sended
  Serial.println("PUBLISH");                            // print infos to SerialPort
  Serial.print("Topic: "); Serial.println(topic);
  Serial.print("Payload: "); Serial.println(payload);
  itoa(payload, mqttPayload, 10);                       // Konvertierung Integer zu Char- Array
  client.publish(topic, mqttPayload, retain);           // publish the message
}

/**
 * @brief Diese Methode publiziert eine MQTT Nachricht. Fliesskommazahl (Float) als Payload. DerPayload wird auf 3 Nachkommastellen gekürzt
 * 
 * @param topic Topic (Name) der Nachricht
 * @param payload Payload (Nutzdaten/Inhalt) der Nachricht
 * @param retain Soll die Nachricht gespeichert werden, auch wenn das Topic niemand abonniert hat.
 */
void wio_mqtt::publishTopic_Float(const char *topic, float payload, bool retain)
{
  char mqttPayload[20];
  pubState = true;                                      // set publish state to TRUE, because somthing will be sended
  Serial.println("PUBLISH");                            // print infos to SerialPort
  Serial.print("Topic: "); Serial.println(topic);
  Serial.print("Payload: "); Serial.println(payload);
  sprintf(mqttPayload, "%.3f", payload);                // Konvertierung Float zu Char- Array mit 3 Nachkommastellen
  client.publish(topic, mqttPayload, retain);           // publish the message
}

/**
 * @brief Diese Methode abboniert ein Topic
 * 
 * @param topic Topicname (Name der zu abbonierenden Nachicht)
 */
void wio_mqtt::subscribeTopic(char *topic)
{
  client.subscribe(topic);    // subscribe the topic      
}

/**
 * @brief Diese Methode speichert Informationen zu der Subscribe-Liste.
 * 
 * @param list Adresse der Subscribe-Liste
 * @param len Länge der Liste (Anzahl Zeichen --> Anzahl Topics x TOPIC_LENGTH)
 */
void wio_mqtt::addSubscribeList(char *list, unsigned int len)
{
  ptr_topicList = list;   // save list address
  topicListLen = len;     // save list length
}

/**
 * @brief Diese Methode abboniert die Subscribe-Liste.
 * 
 * @param list Adresse der Subscribe-Liste
 * @param len Länge der Subscribe-Liste
 */
void wio_mqtt::subscribeList(char *list, unsigned int len)
{
  Serial.println("+++ Subscribe List +++");   // print topics in list to SerialPort
  Serial.print(len);
  Serial.print(" - Liste: ");
  Serial.println(list);
  
  unsigned int topic_cnt = len/TOPIC_LENGTH;     // calc number of topics
  char topic[TOPIC_LENGTH];
  
  for(unsigned int i=0; i<topic_cnt; i++)       // iterate through Subscribe-List
  {
    strcpy(topic, (const char*)list);
    client.subscribe(topic);                  // subscribe topic
    Serial.print("Topic ");                   // print topics in list to SerialPort
    Serial.print(i);
    Serial.print(": ");
    Serial.println(topic);
    for(int ii=0; ii<TOPIC_LENGTH; ii++)    // increment to the next topic 
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
  return client.connected();    // read connection state
}

/**
 * @brief Diese Methode stellt die Verbindung zum Broker wieder her.
 * 
 */
void wio_mqtt::reconnect()
{
  //while (!client.connected())     // Loop until we're reconnected
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "WioTerminal";              // generate id
    clientId += String(random(0xffff), HEX);
    Serial.print(" - ");
    Serial.print(clientId);
    Serial.print(" -...");
    sprintf(logText, "- ID: %s", clientId.c_str());   // write to the log 
    (*cbMQTTLog)(logText, false);
    
    
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      (*cbMQTTLog)("- Connected", false);               // write to the log
      Serial.println("connected");
      subscribeList(ptr_topicList, topicListLen);       // subscribe the topic list
      (*cbMQTTLog)("- Subscribed to Topics", false);    // write to the log
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      //delay(5000);
    }
  }
}

/**
 * @brief Diese Methode verarbeitet eingehende und ausgehende Nachrichten und frischt die Verbindung auf.
 * Damit die MQTT Funktionen ordentlich funktionieren, muss diese Methode periodisch aufgerufen werden.
 * 
 */
void wio_mqtt::clientLoop()
{
  client.loop();
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