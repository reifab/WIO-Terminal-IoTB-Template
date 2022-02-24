/**
 * @file main.cpp
 * @author Beat Sturzenegger
 * @brief Main File vom WIO Terminal Template
 * Du kannst deinen Code zwischen <tt>"START USER CODE"</tt> und <tt>"END USER CODE"</tt>
 * @version 1.2
 * @date 18.01.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 * 
 * 
 * @todo Handling, wenn WiFi verbindung weg ist. Steckt momentan in Endlosschleife bei @ref wio_mqtt.reconnect()
 * @todo testen Verhalten bei MQTT broker unterbruch
 * @todo Automatisches Verbinden mit WiFi
 * 
 */

/********************************************************************************************
*** Includes
********************************************************************************************/
#include <Arduino.h>          // Arduino Library
#include "wio_wifi.h"         // Library for WiFi usage
#include "wio_mqtt.h"         // Library for MQTT usage
#include "wio_display.h"      // Library for Display usage
#include "secrets.h"
#include "pages.h"

// START USER CODE: Includes


// END USER CODE: Includes

/********************************************************************************************
*** Defines
********************************************************************************************/
// MQTT und WiFI defines
#define CONNECTED     1     ///< Verbindungsstatus für MQTT and WiFi
#define DISCONNECTED  0     ///< Verbindungsstatus für MQTT and WiFi

// START USER CODE: Defines


// END USER CODE: Defines

/********************************************************************************************
*** Functionprototypes
********************************************************************************************/
void periodicTasks();
void callback(char* topic, byte* payload, unsigned int length);
void addLogText(const char *text, bool append);

// START USER CODE: Functionprototypes


// END USER CODE: Functionprototypes

/********************************************************************************************
*** Global Parameters
********************************************************************************************/
// WLAN parameters
int wlan_status = DISCONNECTED;   ///< WLAN Status (connected, disconnected, ...)
char wlan_ssid[20] = "-";         ///< WLAN SSID
char wlan_ip[16] = "-";           ///< IP-Adresse des WIO Terminal
int wlan_strength = 0;            ///< Aktuelle WLAN Signalstärke
int wlan_channel = 0;             ///< Benutze WLAN Kanal, wird genutzt für die Frequenzbanderkennung

// MQTT parameters
int mqtt_status = DISCONNECTED;   ///< MQTT Status (connected, disconnected, ...)
bool mqtt_pub_status = false;     ///< MQTT Publish Status
bool mqtt_sub_status = false;     ///< MQTT Subscribe Status
char topicList[][TOPIC_LENGTH] =  ///< Liste der MQTT Topics, die abboniert werden sollen. Die Topics werden als String eingegeben. Das Letzte Zeichen sollte ein '\0' sein. 
                                  ///< Die einzelnen Strings werden mit einem Komma getrennt (Siehe Beispiel. Coming soon...) 
                                  ///< @attention Maximale 50 Zeichen pro Topic!
                                  ///< @todo Beispiel für MQTT
{
  // START USER CODE: Subscribed Topics
  "Example\0"   // Beispiel für ein Topic, Bitte löschen, wenn eigene Topics hinzugefügt werden

  // END USER CODE: Subscribed Topics
};

// Display Parameter
extern page_t pages_array[];      ///< extern Page Array, is coded in pages.c
int currentPage = 1;              ///< current page, needed for button actions
char log_text[15][TOPIC_LENGTH];            ///< startup log text, 15 lines with 50 symbols

// intervals for periodic tasks
const long scanInterval = 10000;            ///< Interval time for WiFi strength und channel scanning 
const long interfaceIconIntervall = 500;    ///< Interval time for icons refreshing
const long mqttStateIntervall = 1000;       ///< Interval time for MQTT state
const long userDefinedIntervall = 5000;     ///< Interval time for user defined perdiodic task

// START USER CODE: Global Variables


// END USER CODE: Global Variables

/********************************************************************************************
*** Objects
********************************************************************************************/
wio_display wio_disp = wio_display(&mqtt_status, &mqtt_pub_status, &mqtt_sub_status, &wlan_status, &wlan_strength, &wlan_channel);
wio_wifi wio_Wifi = wio_wifi(addLogText);
wio_mqtt wio_MQTT = wio_mqtt(addLogText);

// START USER CODE: Objects


// END USER CODE: Objects


/********************************************************************************************
*** Setup Function
********************************************************************************************/

/**
 * @brief Setup Funktion \n
 * Wird einmal vor der Main Funktion loop ausgeführt.
 * 
 */
void setup() {
  Serial.begin(115200);                 // init Serial Port with 115200 baud
  delay(1000);                          // 1000ms delay for Serial begin

  // Display Initialization
  Serial.println("init Display");     
  wio_disp.initDisplay();               // init built-in display
  wio_disp.loadingScreen(1);            // enable loading screen

  // WiFi Configuration
  Serial.println("init WiFi");
  wio_Wifi.initWifi();                  // init WiFi
  wlan_status = wio_Wifi.WiFiStatus();  // get WiFi/WLAN connection Status
  wio_Wifi.scanNetwork();               // start network scan (asynchronus)

  // MQTT Configuration
  Serial.println("init MQTT");
  wio_MQTT.addSubscribeList(topicList[0], sizeof(topicList));  // subscribe to all topics in topicList
  wio_MQTT.initMQTT(callback);          // init MQTT

  // Button Configuration
  Serial.println("config Buttons");
  addLogText("Config Buttons", false);
  pinMode(WIO_KEY_A, INPUT_PULLUP);     // config button <A>
  pinMode(WIO_KEY_B, INPUT_PULLUP);     // config button <B>
  pinMode(WIO_KEY_C, INPUT_PULLUP);     // config button <C>
  pinMode(WIO_5S_UP, INPUT_PULLUP);     // config 5-way-switch <UP>
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);   // config 5-way-switch <DOWN>
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);   // config 5-way-switch <LEFT>
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);  // config 5-way-switch <RIGHT>
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);  // config 5-way-switch <PRESS>

  addLogText("Init successfully!", false);
  delay(5000);                          // 5000ms delay
  wio_disp.loadingScreen(0);            // disable Loading screen

  // START USER CODE: Setup
  currentPage = 0;                              // set currentPage
  wio_disp.drawPage(pages_array[currentPage]);  // draw Page No. 1 (= currentPage)

  // END USER CODE: Setup
}


/********************************************************************************************
*** Loop Function
********************************************************************************************/

/**
 * @brief Main Funktion \n
 * Wird immer wieder ausgeführt. Auch ohne einer Endlosschleife wie @p while(1) @emoji :wink:
 * 
 */
void loop() {
  //================================//
  // Button Handler                 //
  //================================//
  if (digitalRead(WIO_5S_DOWN) == LOW)    // 5-Way-Switch slided Down
  {
    delay(100);
    switch(currentPage)
    {
      case 1:
        // START USER CODE: PAGE 1 
        
        // END USER CODE: PAGE 1
        break;

      default:
        break;
    }
    
  }
  else if (digitalRead(WIO_5S_UP) == LOW)   // 5-Way-Switch slided Up
  {
    delay(100);
    switch(currentPage)
    {
      case 1:
        // START USER CODE: PAGE 1 
        
        // END USER CODE: PAGE 1
        break;

      default:
        break;
    }
  }
  else if (digitalRead(WIO_5S_PRESS) == LOW)    // 5-Way-Switch pressed
  {
    delay(100);
    switch(currentPage)
    {
      case 1:
        // START USER CODE: PAGE 1 
        
        // END USER CODE: PAGE 1
        break;
     
     default:
        break;
    }
  }
  else if (digitalRead(WIO_5S_RIGHT) == LOW)    // 5-Way-Switch slided to Right
  {
    delay(100);
    switch(currentPage)
    {
      case 1:
        // START USER CODE: PAGE 1 
        
        // END USER CODE: PAGE 1
        break;
     
     default:
        break;
    }
  }
  else if (digitalRead(WIO_5S_LEFT) == LOW)    // 5-Way-Switch slided to Left
  {
    delay(100);
    switch(currentPage)
    {
      case 1:
        // START USER CODE: PAGE 1 
        
        // END USER CODE: PAGE 1
        break;
     
     default:
        break;
    }
  }
  else if (digitalRead(WIO_KEY_A) == LOW)   // Button A pressed
  {
    delay(100);
    switch(currentPage)
    {
      case 1:
        // START USER CODE: PAGE 1 
        
        // END USER CODE: PAGE 1
      default:
        break;
    } 
  }
  else if (digitalRead(WIO_KEY_B) == LOW)   // Button B pressed
  {
    delay(100);
    switch(currentPage)
    {
      case 1:
        // START USER CODE: PAGE 1 
        
        // END USER CODE: PAGE 1
      default:
        break;
    }   
  }
  else if (digitalRead(WIO_KEY_C) == LOW)   // Button C pressed
  {
    delay(100);
    switch(currentPage)
    {
      case 1:
        // START USER CODE: PAGE 1 
        
        // END USER CODE: PAGE 1
      default:
        break;
    }   
  }

  
  //================================//
  // Page dependet tasks            //
  //================================//
  switch(currentPage)
  {
    case 0:
      // START USER CODE: Specific Page 0 Code
      

      // END USER CODE: Specific Page 0 Code
      break;

    case 1:
      // START USER CODE: Specific Page 1 Code
      

      // END USER CODE: Specific Page 1 Code
      break;
    
    // START USER CODE: more Page specific Code


    // END USER CODE: more Page specific Code
  }

  // START USER CODE: loop Code


  // END USER CODE: loop Code

  //================================//
  // Periodic tasks                 //
  //================================//
  periodicTasks();
}

/********************************************************************************************
*** Functions
********************************************************************************************/

/**
 * @brief In dieser Funktion werden Aufgaben und Funktionen, nach Ablauf eines
 * bestimmten Intervalls, ausgeführt.
 * @note Für eigene periodische Funktionen muss das Array <tt> @b previousMillis </tt> erweitert werden.\n
 * @note <tt> @b previousMillis[3] </tt> und <tt> @b userDefinedIntervall </tt> können für eine eigene periodische Aufgabe benutzt werden.
 * 
 */
void periodicTasks()
{
  static long previousMillis[] = {0,0,0,0};   // array for the differents previous millis value
                                              // previousMillis[0]: WiFi scan
                                              // previousMillis[1]: update MQTT states
                                              // previousMillis[2]: update Interface icons on the display
                                              // previousMillis[3]: for user
  static int networksFound = -3;            // number of founded networks; -3: start value, -2: scan not triggered, -1: scan not completed
  long currentMillis = millis();            // save millis

#ifdef WIO_WIFI_H
  if(wlan_status == CONNECTED)    // is connected to WLAN?
  {
    if((currentMillis - previousMillis[0] >= scanInterval) || previousMillis[0] == 0)
    {
      previousMillis[0] = currentMillis;
      networksFound = wio_Wifi.getNetworksFound();
      if(networksFound <= -2)       // if scan not triggered?
      {
        wio_Wifi.scanNetwork();     // start network scan (asynchronus)
      }
  
      if(networksFound > 0)     // is Scan Done?
      {
        // read No. of current WiFi
        for (int i = 0; i < networksFound; i++)
        {
          //Serial.printf("%d: %s, Ch:%d (%ddBm) %s\n", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == ENC_TYPE_NONE ? "open" : ""); // print information of the selected WiFi
          if (strcmp(wio_Wifi.getScannedSSID(i), wio_Wifi.getSSID()) == 0)    // is the selexted WiFi the same as the connected WiFi?
          {
            wlan_strength = wio_Wifi.measureRSSI(i);  // read RSSI
            wlan_channel = wio_Wifi.readChannel(i);   // read WiFi channel
            break;
          }
          wio_Wifi.deleteScanResults();             // delete scan results and reset scan progress
        }
      }
    }
  }
#endif 

#ifdef WIO_MQTT_H
  if((currentMillis - previousMillis[1] >= mqttStateIntervall) || previousMillis[1] == 0)
  {
    if(wlan_status)
    {
      if(wio_MQTT.isConnected())    // check MQTT connection
      {
        mqtt_status = CONNECTED;
        if(mqtt_status)
        {
          wio_MQTT.clientLoop();    // MQTT client loop
          previousMillis[1] = currentMillis;                // refresh previousMillis
          Serial.println("- update MQTT State");            // write Info to SerialPort
          mqtt_pub_status = wio_MQTT.getPublishState();     // get publish state
          wio_MQTT.setPublishState(false);                  // reset publish state in the mqtt library
          mqtt_sub_status = wio_MQTT.getSubscribeState();   // get subscribe state
          wio_MQTT.setSubscribeState(false);                // reset subscribe state in the mqtt library
        }
      }
      else
      {
        mqtt_status = DISCONNECTED;
        wio_MQTT.reconnect();         // reconnect to MQTT broker
      }
    }
    else
    {
      mqtt_status = DISCONNECTED;
    }
  }
#endif 

  if((currentMillis - previousMillis[2] >= interfaceIconIntervall) || previousMillis[2] == 0)
  {
    previousMillis[2] = currentMillis;      // refresh previousMillis
    Serial.println("- update Icons");       // write Info to SerialPort
    wlan_status = wio_Wifi.WiFiStatus();    // read WiFi status
    if(wlan_status == DISCONNECTED)
    {
      wlan_strength = 99;
      wlan_channel = 0;
      wio_Wifi.reconnect();
    }
    else
    {
      if(wlan_strength == 99)
      {
        wlan_strength = -99;
      }
    }
    wio_disp.updateInterfaceStatus();       // update Interface status on the display
  }

  if((currentMillis - previousMillis[3] >= userDefinedIntervall) || previousMillis[3] == 0)
  {
    previousMillis[3] = currentMillis;      // refresh previousMillis
    // START USER CODE: user periodic task
      

    // END USER CODE: user periodic task
  }
}






/**
 * @brief MQTT Callback Function.
 * Diese Funktion wird automatisch aufgerufen, wenn sich ein abboniertes MQTT Topic ändert.\n
 * Im String <tt>@b payload_str</tt> steht der Inhalt des Topics.
 * 
 * 
 * @param topic Adresse von Topicname, "wo steht der Topicname"
 * @param payload Adresse vom Topicinhalt, "wo steht der Inhalt"
 * @param length  Länge vom Inhalt, wie viele Zeichen hat der Inhalt
 */
void callback(char* topic, byte* payload, unsigned int length)
{
  int elem = 0;
  wio_MQTT.setSubscribeState(true);     // set subscribe state (for display)
  
  char payload_str[length];
  for (unsigned int i = 0; i < length; i++)
  {
    payload_str[i] = (char)payload[i];      // convert payload to a string
  }
  payload_str[length] = '\0';               // add string endbyte
  
  while(strcmp(topicList[elem], topic))     // search topic in topicList
  {
      elem++;     // if not found, increment elem
  }
  
  switch(elem)
  {
    case 0:
      // START USER CODE: first element of the topicList
      Serial.println(payload_str);  // Beispiel

      // END USER CODE: first element of the topicList
      break;
      
    case 1:
      // START USER CODE: second element of the topicList


      // END USER CODE: second element of the topicList
      break;

    // START USER CODE: more elements of the topicList


    // END USER CODE: more elements of the topicList
  }
}

/**
 * @brief Diese Funktion fügt einen Text zum Log Text hinzu
 * 
 * @param log_ Text der hinzugefügt werden soll
 * @param append Soll der Text angehängt ( @p true ) oder auf eine neue Zeile geschrieben ( @p false ) werden
 */
void addLogText(const char *text, bool append){
  wio_disp.addLogText(text, append);
}

// START USER CODE: user functions
      

// END USER CODE: user functions
