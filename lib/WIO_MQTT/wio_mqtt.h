/**
 * @file wio_mqtt.h
 * @author Beat Sturzenegger / Fabian Reifler
 * @brief IoTB MQTT Bibliothek für das WIO Terminal
 * @version 1.3
 * @date 18.01.2022
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef WIO_MQTT_H
#define WIO_MQTT_H
/********************************************************************************************
*** Defines
********************************************************************************************/
#define TOPIC_LENGTH 50 ///< Maximale Länge von einem Topic
#define BUFFER_LENGTH 50 ///< Maximale Länge eines Puffers

/********************************************************************************************
*** Interface description
********************************************************************************************/
class wio_mqtt
{
public:
  typedef void (*message_cb)(char *topic, byte *payload, unsigned int len);

public:
  wio_mqtt(void (&)(const char *, bool));                                 ///< Konstruktor
  void initMQTT(message_cb callback, const char *mqtt_user, const char *mqtt_password, const char *wioTerminalID);///< MQTT initialisieren
  void publishTopic(const char *topic, const char *payload, bool retain); ///< Ein Topic publizieren, Payload ist ein konstanter String
  void publishTopic(const char *topic, int payload, bool retain);         ///< Ein Topic publizieren, Payload ist ein Integer
  void publishTopic(const char *topic, float payload, bool retain);       ///< Ein Topic publizieren, Payload ist ein Fliesskommazahl
  void publishTopicPrintf(const char *topic, bool retain, const char *format, ...);
  void setCredentials(const char *user, const char *pass);
  void subscribeTopic(char *topic);                                       ///< Ein Topic abonnieren
  void addSubscribeList(char *list, unsigned int len);                    ///< Topic-Liste zur Klasse hinzufügen
  bool isConnected(void);                                                 ///< MQTT Verbindung auslesen
  void reconnect(const char *mqtt_broker, uint16_t mqtt_port);            ///< Wiederverbindung zum MQTT Broker
  void loop(void);                                                        ///< MQTT loop für einen ordnungsgemässer Betrieb
  bool getPublishState();
  void resetPublishState();
  bool getSubscribeState();                                               ///< Den Subscribe Status auslesen
  void setSubscribeState(bool state);                                     ///< Den Subscribe Status setzen

protected:

private:
  String m_clientId;
  String m_user;
  String m_pass;

  bool m_publishedSomething;

  char *ptr_topicList;                           ///< Pointer zu der Topic Liste
  unsigned int topicListLen = 0;                 ///< Länge der Topic Liste
  bool subState = false;                         ///< Subscribe Status
  typedef void (*callbackFunc) (char *topic, byte *payload, unsigned int len);    ///< Functionspointer auf die Callback Funktion
  callbackFunc _callback;
  char m_log_text[BUFFER_LENGTH];
  typedef void (*cbLog)(char *s, bool b);
  static cbLog _cbLog;                              ///< Callback Funktionsvariable
  void subscribeList(char *list, unsigned int len); ///< Eine Topic-Liste abonnieren
};

#endif
