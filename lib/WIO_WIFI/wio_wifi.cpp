/**
 * @file wio_wifi.cpp
 * @author Beat Sturzenegger
 * @brief IoTB WiFi Bibliothek für das WIO Terminal
 * @version 1.2
 * @date 18.01.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/********************************************************************************************
*** Defines and Constants
********************************************************************************************/
#define DEBUG_ON 1
#define BUFFER_LENGTH 50

/********************************************************************************************
*** Includes
********************************************************************************************/
#include "wio_wifi.h"
#include <rpcWiFi.h>
#include <utils.h>

/********************************************************************************************
*** Variables
********************************************************************************************/
static bool interface_ready = false;
static IPAddress ip;
static char logText[BUFFER_LENGTH];
static wio_wifi_log_cb logPrintStrategy;
static char ssid[50];
static char password[50];

/********************************************************************************************
*** Functionprototypes
********************************************************************************************/
static void PrintLog(const char *str, bool append);
void onWiFiEvent(WiFiEvent_t event);

/********************************************************************************************
*** Constructor
********************************************************************************************/

/**
 * @brief Konstruktor
 * 
 * @param func Funktionsadresse der Funktion für das Hinzufügen des Log Text
 */
wio_wifi::wio_wifi(wio_wifi_log_cb log_cb)
{
  logPrintStrategy = log_cb;
}

/********************************************************************************************
*** Public Methodes
********************************************************************************************/

/**
 * @brief Diese Methode setzt die Wifi-Parameter und stellt eine Verbindung zum Access Point her.
 * 
 */
void wio_wifi::initWifi(const char* wifi_ssid, const char* wifi_password)
{
  /****************************************** **************************************************
*** Variables
********************************************************************************************/
  strcpy(ssid, wifi_ssid);
  strcpy(password, wifi_password);

  PrintLog("Start WiFi Init", false);

  if(DEBUG_ON){
    WiFi.onEvent(onWiFiEvent);
  }

  WiFi.disconnect(true,false);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);

  reconnect();
}

/**
 * @brief Diese Methode verbindet sich mit dem Access Point
 * 
 */
void wio_wifi::reconnect(void)
{
  if (WiFi.status() != WL_DISCONNECTED) {
    WiFi.disconnect(true,false);
  }

  snprintf_safe(logText, BUFFER_LENGTH, "Connecting to %s...", ssid);
  PrintLog(logText, false);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)  // do begin WiFi connection until connected
  {
    delay(200);
    PrintLog(".", true);
  }
  WiFi.scanNetworks();
  PrintLog("- Connected", false);
}

/**
 * @brief Diese Methode gibt die Signalstärke (RSSI) zurück.
 * @note Es wird Empfohlen vorher das Netzwerk zu scannen mit @ref scanNetwork() , damit man 
 * herausfinden kann welcher Indexnummer die verbundene Verbindung hat.
 * 
 * @param index Indexnummer des Netzwerkes
 * @return int RSSI als integer Wert
 */
int wio_wifi::measureRSSI(int index)
{
  long rssi = WiFi.RSSI(index);   // read rssi
  return (int)rssi;               // parse and return rssi
}

/**
 * @brief Diese Methode liest den WLAn Kanal aus.
 * @note Es wird Empfohlen vorher das Netzwerk zu scannen mit @ref scanNetwork() , damit man 
 * herausfinden kann welcher Indexnummer die verbundene Verbindung hat.
 * 
 * @param index Indexnummer des Netzwerkes
 * @return int WLAN Kanal als integer Wert
 */
int wio_wifi::readChannel(int index)
{
  long ch = WiFi.channel(index);    // read channel
  return (int)ch;                   // parse and return channel
}

/**
 * @brief Diese Methode scannt nach @b sichtbaren WLAN Verbindungen.
 * @note Der Scan wird im asynchronen Modus durchgeführt. Mit der Methode @ref getNetworksFound() 
 * kann der aktuelle Scan Status oder die gefundenen Netzwerke ausgelesen werden.
 */
void wio_wifi::scanNetwork(void)
{
  WiFi.scanNetworks();   // Scan Networks in asynchronus mode and without hidden SSIDs
}

/**
 * @brief Diese Methode gibt den aktuellen Scan Status zurück oder, bei abgeschlossenem Scan, die Anzahl der gefundenen Netzwerke zurück.
 * 
 * @return int Anzahl gefundene Netzwerke oder Scan Status
 * - -2: Scan wurde noch nicht getriggert/ausgelöst
 * - -1: Scan ist noch nicht abgeschlossen
 * - >0: Anzahl der gefundenen Netzwerke
 */
int wio_wifi::getNetworksFound(void)
{
  return WiFi.scanComplete();    // check if scan is completed, if complete
}

/**
 * @brief Diese Methode gibt, anhand der Indexnummer der gescannten Netzwerke, den SSID Namen zurück.
 * 
 * @param index Indexnummer des Netzwerkes
 * @return char* SSID-Namen als String
 */
char *wio_wifi::getScannedSSID(int index)
{
  return (char *)WiFi.SSID(index).c_str();
}

/**
 * @brief Diese Methode gibt den SSID Namen zurück.
 * 
 * @return const char* SSID Name als String
 */
const char *wio_wifi::getSSID()
{
  return ssid;
}

/**
 * @brief Diese Methode löscht die Scan Resultate.
 * 
 */
void wio_wifi::deleteScanResults()
{
  WiFi.scanDelete();
}

/********************************************************************************************
*** Private Methodes
********************************************************************************************/

/**
 * @brief Diese Funktion reagiert auf WiFi Events und gibt diesen Event auf das Log und SerialPort aus.
 * 
 * @param event Der Event der ausgelöst wurde.
 */
void onWiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_WIFI_READY: 
      PrintLog("- WiFi interface ready", false);
      interface_ready = true;
      break;
    case SYSTEM_EVENT_STA_START:
      PrintLog("- WiFi client started", false);
      break;
    case SYSTEM_EVENT_STA_STOP:
      PrintLog("- WiFi clients stopped", false);
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      PrintLog("- WiFi Connected", false);
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      PrintLog("- WiFi Disconnected", false);
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
      PrintLog("- SYSTEM_EVENT_STA_AUTHMODE_CHANGE", false);
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      ip = WiFi.localIP();
      snprintf(logText, BUFFER_LENGTH, "- IP Adress: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
      PrintLog(logText, false); 
      break;
    case SYSTEM_EVENT_STA_LOST_IP:
      PrintLog("- STA_LOST_IP", false);
      break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      PrintLog("- STA_WPS_ER_SUCCESS", false);
      break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      PrintLog("- STA_WPS_ER_FAILED", false);
      break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      PrintLog("- STA_WPS_ER_TIMEOUT", false);
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      PrintLog("- STA_WPS_ER_PIN", false);
      break;
    case SYSTEM_EVENT_AP_START:
      PrintLog("- WiFi access point started", false);
      break;
    case SYSTEM_EVENT_AP_STOP:
      PrintLog("- WiFi access point stopped", false);
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      PrintLog("- Client connected", false);
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      PrintLog("- Client disconnected", false);
      break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
      PrintLog("- Assigned IP address to client", false);
      break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
      PrintLog("- Received probe request", false);
      break;
    case SYSTEM_EVENT_GOT_IP6:
      PrintLog("- IPv6 is preferred", false);
      break;
    case SYSTEM_EVENT_ETH_START:
      PrintLog("- ETH started", false);
      break;
    case SYSTEM_EVENT_ETH_STOP:
      PrintLog("- ETH stopped", false);
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      PrintLog("- ETH connected", false);
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      PrintLog("- ETH disconected", false);
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      PrintLog("- ETH got IP", false);
      break;
    default: break;      
  }
}

/**
 * @brief Diese Methode gibt die IP-Adresse des Gerätes zurück.
 * 
 * @param addr Stringadresse, für die Speicherung, also wohin wird die IP-Adresse "geschrieben"
 * @param len Länge des Puffers auf den mit @p addr gezeigt wird.
 */
int wio_wifi::getIP(char *addr, size_t len)
{
  return snprintf_safe(addr, len, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

/**
 * @brief Diese Methode gibt den WiFi Status zurück
 * 
 * @return int WiFi Status
 * - @p 0: Unterbrochen
 * - @p 1: Verbunden
 */
int wio_wifi::WiFiStatus()
{
  if(WiFi.status() == WL_CONNECTED)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

static void PrintLog(const char *str, bool wantToAppendMore) {
  if (wantToAppendMore) {
    Serial.print(str);
  } else {
    Serial.println(str);
  }

  logPrintStrategy(str, wantToAppendMore);
}
