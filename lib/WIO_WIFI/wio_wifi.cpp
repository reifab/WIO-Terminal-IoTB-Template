/**
 * @file wio_wifi.cpp
 * @author Beat Sturzenegger
 * @brief IoTB WiFi Bibliothek für das WIO Terminal
 * @version 1.1
 * @date 18.01.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/********************************************************************************************
*** Includes
********************************************************************************************/
#include "wio_wifi.h"
#include <rpcWiFi.h>

/********************************************************************************************
*** Variables
********************************************************************************************/
static bool interface_ready = false;
static IPAddress ip;
static char logText[50];
typedef void (*cbLog)  (const char *s, bool b);
static cbLog cbWiFiLog;


/********************************************************************************************
*** Functionprototypes
********************************************************************************************/
void onWiFiEvent(WiFiEvent_t event);

/********************************************************************************************
*** Constructor
********************************************************************************************/

/**
 * @brief Konstruktor
 * 
 * @param func Funktionsadresse der Funktion für das Hinzufügen des Log Text
 */
wio_wifi::wio_wifi(void (&func)(const char *, bool b))
{
  cbWiFiLog = func;
}

/********************************************************************************************
*** Public Methodes
********************************************************************************************/

/**
 * @brief Diese Methode setzt die Wifi-Parameter und stellt eine Verbindung zum Access Point her.
 * 
 */
void wio_wifi::initWifi(void)
{
  wifiSetup();      // activate WiFi Events

  (*cbWiFiLog)("Start WiFi Init", false);   // write to the log
  WiFi.disconnect();                        // diconnect any connection
  delay(100);

  sprintf(logText, "- Connecting to %s", ssid);   // write to the log 
  (*cbWiFiLog)(logText, false);

  Serial.println(ssid);                     // print to SerialPort
  WiFi.begin(ssid, password);               // begin WiFi connection

  delay(100);
  while (WiFi.status() != WL_CONNECTED)       // do until connected
  {
    delay(500);
    Serial.println("Connecting to WiFi..");   // print to SerialPort
    WiFi.begin(ssid, password);               // begin WiFi connection
  }
 
  (*cbWiFiLog)("- Connected", false);       // write to the log
}

/**
 * @brief Diese Methode verbindet sich mit dem Access Point
 * 
 */
void wio_wifi::reconnect(void)
{
  Serial.println("Reconnect WiFi");
  WiFi.disconnect();                        // diconnect any connection
  WiFi.begin(ssid, password);
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
  WiFi.scanNetworks(true, false);   // Scan Networks in asynchronus mode and without hidden SSIDs
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
      //(*cbWiFiLog)("*WiFi interface ready", false);
      Serial.println("WiFi interface ready");
      interface_ready = true;
      break;
    case SYSTEM_EVENT_STA_START:
      (*cbWiFiLog)("- WiFi client started", false);
      Serial.println("WiFi client started");
      break;
    case SYSTEM_EVENT_STA_STOP:
      (*cbWiFiLog)("- WiFi clients stopped", false);
      Serial.println("WiFi clients stopped");
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      (*cbWiFiLog)("- Connected to access point", false);
      Serial.println("Connected to access point");
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      (*cbWiFiLog)("- Disconnected from WiFi access point", false);
      Serial.println("Disconnected from WiFi access point");
      //WiFi.begin(ssid, password);
      break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
      (*cbWiFiLog)("- Authentication mode of access point has changed", false);
      Serial.println("Authentication mode of access point has changed");
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      ip = WiFi.localIP();
      sprintf(logText, "- IP Adress: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
      (*cbWiFiLog)(logText, false);
      break;
    case SYSTEM_EVENT_STA_LOST_IP:
      (*cbWiFiLog)("- STA_LOST_IP", false);
      Serial.println("Lost IP address and IP address is reset to 0");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      (*cbWiFiLog)("- STA_WPS_ER_SUCCESS", false);
      Serial.println("WiFi Protected Setup (WPS): succeeded in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      (*cbWiFiLog)("- STA_WPS_ER_FAILED", false);
      Serial.println("WiFi Protected Setup (WPS): failed in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      (*cbWiFiLog)("- STA_WPS_ER_TIMEOUT", false);
      Serial.println("WiFi Protected Setup (WPS): timeout in enrollee mode");
      break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
      (*cbWiFiLog)("- STA_WPS_ER_PIN", false);
      Serial.println("WiFi Protected Setup (WPS): pin code in enrollee mode");
      break;
    case SYSTEM_EVENT_AP_START:
      (*cbWiFiLog)("- WiFi access point started", false);
      Serial.println("WiFi access point started");
      break;
    case SYSTEM_EVENT_AP_STOP:
      (*cbWiFiLog)("- WiFi access point stopped", false);
      Serial.println("WiFi access point stopped");
      break;
    case SYSTEM_EVENT_AP_STACONNECTED:
      (*cbWiFiLog)("- Client connected", false);
      Serial.println("Client connected");
      break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
      (*cbWiFiLog)("- Client disconnected", false);
      Serial.println("Client disconnected");
      break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
      (*cbWiFiLog)("- Assigned IP address to client", false);
      Serial.println("Assigned IP address to client");
      break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
      (*cbWiFiLog)("- Received probe request", false);
      Serial.println("Received probe request");
      break;
    case SYSTEM_EVENT_GOT_IP6:
      (*cbWiFiLog)("- IPv6 is preferred", false);
      Serial.println("IPv6 is preferred");
      break;
    case SYSTEM_EVENT_ETH_START:
      (*cbWiFiLog)("- ETH started", false);
      Serial.println("Ethernet started");
      break;
    case SYSTEM_EVENT_ETH_STOP:
      (*cbWiFiLog)("- ETH stopped", false);
      Serial.println("Ethernet stopped");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      (*cbWiFiLog)("- ETH connected", false);
      Serial.println("Ethernet connected");
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      (*cbWiFiLog)("- ETH disconected", false);
      Serial.println("Ethernet disconnected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      (*cbWiFiLog)("- ETH got IP", false);
      Serial.println("Obtained IP address");
      break;
    default: break;      
  }
}

/**
 * @brief Diese Methode aktiviert die WiFi Events
 * 
 */
void wio_wifi::wifiSetup() {
  WiFi.onEvent(onWiFiEvent);
}

/**
 * @brief Diese Methode gibt die IP-Adresse des Gerätes zurück.
 * 
 * @param addr Stringadresse, für die Speicherung, also wohin wird die IP-Adresse "geschrieben"
 */
void wio_wifi::getIP(char *addr)
{
  char ip_text[16];
  
  sprintf(ip_text, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  strcpy(addr, ip_text);
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
