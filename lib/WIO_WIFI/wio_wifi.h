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
*** Extern Variables
********************************************************************************************/
extern const char* ssid;        ///< defined in secrets.h
extern const char* password;    ///< defined in secrets.h

/********************************************************************************************
*** Interface description
********************************************************************************************/
class wio_wifi
{
  public:
    wio_wifi(void (&)(const char *, bool));   ///< Konstruktor
    void initWifi(void);                ///< WiFi initialisieren
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
  private:
    
    
    void wifiSetup();
};

#endif
