/**
 * @file wio_mqtt.h
 * @author Beat Sturzenegger
 * @brief IoTB MQTT Bibliothek für das WIO Terminal
 * @version 1.3
 * @date 18.01.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef WIO_MQTT_H
#define WIO_MQTT_H
/********************************************************************************************
*** Defines
********************************************************************************************/
#define TOPIC_LENGTH  50    ///< Maximale Länge von einem Topic

/********************************************************************************************
*** Extern Variables
********************************************************************************************/
extern const char *mqtt_server;     ///< defined in secrets.h
extern const char *mqtt_user;       ///< defined in secrets.h
extern const char *mqtt_password;   ///< defined in secrets.h
extern const char *mqtt_id;         ///< defined in secrets.h

/********************************************************************************************
*** Interface description
********************************************************************************************/
class wio_mqtt
{
  public:
    wio_mqtt(void (&)(const char *, bool));                                               ///< Konstruktor
    void initMQTT(void (&)(char*, byte*, unsigned int));                                  ///< MQTT initialisieren
    void publishTopic_String(const char *topic, char *payload, bool retain);              ///< Ein Topic publizieren, Payload ist ein String
    void publishTopic_ConstString(const char *topic, const char *payload, bool retain);   ///< Ein Topic publizieren, Payload ist ein konstanter String
    void publishTopic_Integer(const char *topic, int payload, bool retain);               ///< Ein Topic publizieren, Payload ist ein Integer
    void publishTopic_Float(const char *topic, float payload, bool retain);               ///< Ein Topic publizieren, Payload ist ein Fliesskommazahl
    void subscribeTopic(char *topic);                                                     ///< Ein Topic abonnieren
    void addSubscribeList(char *list, unsigned int len);                                  ///< Topic-Liste zur Klasse hinzufügen
    bool isConnected(void);                                                               ///< MQTT Verbindung auslesen
    void reconnect(void);                                                                 ///< Wiederverbindung zum MQTT Broker
    void clientLoop(void);                                                                ///< MQTT loop für einen ordnungsgemässer Betrieb 
    bool getPublishState();                                                               ///< Den Publish Status auslesen
    bool getSubscribeState();                                                             ///< Den Subscribe Status auslesen
    void setPublishState(bool state);                                                     ///< Den Publish Status setzen
    void setSubscribeState(bool state);                                                   ///< Den Subscribe Status setzen
  private:
    char *ptr_topicList;                                                            ///< Pointer zu der Topic Liste
    unsigned int topicListLen = 0;                                                  ///< Länge der Topic Liste
    bool pubState = false;                                                          ///< Publish Status
    bool subState = false;                                                          ///< Subscribe Status
    typedef void (*callbackFunc) (char *topic, byte *payload, unsigned int len);    ///< Functionspointer auf die Callback Funktion
    callbackFunc _callback;      
    char logText[50];
    typedef void (*cbLog)  (char *s, bool b);
    static cbLog _cbLog;                                                   ///< Callback Funktionsvariable
    void subscribeList(char *list, unsigned int len);                               ///< Eine Topic-Liste abonnieren
};

#endif
