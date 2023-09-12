/**
 * @file wio_wifi.h
 * @author Beat Sturzenegger
 * @brief IoTB WiFi Bibliothek für das WIO Terminal
 * @version 1.1
 * @date 18.01.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef WIO_WIFI_H
#define WIO_WIFI_H

#include <rpcWiFi.h>

/********************************************************************************************
*** Defines
********************************************************************************************/
// MQTT und WiFI defines
#define CONNECTED 1    ///< Verbindungsstatus für MQTT and WiFi
#define DISCONNECTED 0 ///< Verbindungsstatus für MQTT and WiFi


/********************************************************************************************
*** Interface description
********************************************************************************************/
class wio_wifi
{
  public:
    wio_wifi(void (&)(const char *, bool));   ///< Konstruktor
    void initWifi(const char* ssid, const char* password);///< WiFi initialisieren
    int WiFiStatus();                   ///< WiFi Verbindungsstatus auslesen
    void reconnect();                   ///< verbindet sich wieder mit dem WLAN
    void getIP(char *);                 ///< aktuelle IP Adresse auslesen
    const char *getSSID();              ///< SSID aus dem secret.h File
    void scanNetwork(void);             ///< nach Netzwerken suchen
    void deleteScanResults();           ///< Scan Resultate löschen
    int getNetworksFound(void);         ///< Anzahl der gefundenen Netzwerke auslesen
    int measureRSSI(int);               ///< Signalstärke auslesen
    int readChannel(int);               ///< WLAN Kanal auslesen
    char *getScannedSSID(int index);    ///< SSID des gefunden Netzwerkes auslesen
};

#endif
