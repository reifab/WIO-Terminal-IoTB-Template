/**
 * @file main.cpp
 * @author Beat Sturzenegger / Fabian Reifler
 * @brief Main File vom WIO Terminal Template
 * @version 1.3
 * @date 24.04.2023
 *
 * @copyright Copyright (c) 2023
 *
 */

/********************************************************************************************
*** Includes
********************************************************************************************/
#include <Arduino.h>  // Arduino Library
#include "wio_wifi.h" // Library for WiFi usage
#include "wio_mqtt.h" // Library for MQTT usage
#include "pages.h"
#include "userFunctions.h"
#include "buttons.h"
#include "display.h"
#include "networkConnection.h"
#include "runLED.h"
#include "SAMCrashMonitor.h"
#include "readConfigSDCard.h"

/********************************************************************************************
*** Global Parameters
********************************************************************************************/

// Display Parameter
extern page_t pages_array[];     ///< extern Page Array, is coded in pages.c
uint16_t currentPage = 0;        ///< current page, needed e.g. for button actions
char log_text[15][TOPIC_LENGTH]; ///< startup log text, 15 lines with 50 symbols

/********************************************************************************************
*** Objects
********************************************************************************************/
wio_wifi wio_Wifi = wio_wifi(addLogText);
wio_mqtt wio_MQTT = wio_mqtt(addLogText);

/********************************************************************************************
*** Setup Function
********************************************************************************************/

/**
 * @brief Setup Funktion \n
 * Wird einmal vor der Main Funktion loop ausgeführt.
 *
 */
void setup()
{
  Serial.begin(115200); // init Serial Port with 115200 baud
  delay(1000);          // 1000ms delay for Serial begin

  SAMCrashMonitor::begin();
  SAMCrashMonitor::disableWatchdog(); // Make sure it is turned off during init.
  SAMCrashMonitor::dump();            // Dump watchdog reset data to the console.

  // Display Initialization
  initDisplay();
  enableLoadingScreen();

  // Turn the watchdog on and get actual timeout value based on the provided one.
  SAMCrashMonitor::enableWatchdog(25000);

  // Button Configuration
  initButtons();
  addLogText("Config Buttons", NEWLINE);

  readConfigFromSDCard();
  addLogText(getWifiSSID(), NEWLINE);
  addLogText(getWifiPW(), NEWLINE);

  // WiFi Configuration
  wio_Wifi.initWifi(getWifiSSID(), getWifiPW());
  delay(2000);
  SAMCrashMonitor::iAmAlive(); // init WiFi

  // Initialize user functions like subscrition of mqtt topics
  currentPage = initUserFunctions(&wio_MQTT);

  // Connects to the MQTT Broker and subscribes the topics
  setMQTTUserAndPassword(&wio_MQTT,getMQTTUser(),getMQTTPW());
  changeMQTTBroker(getMQTTBroker(), 1883, getMQTTUser(), getMQTTPW(), &wio_MQTT);

  addLogText("Init successfully!", NEWLINE);
  delay(1000); // 1000ms delay to read the init- message
  disableLoadingScreen();

  drawPage(pages_array, currentPage);

  initRunLed();

  strcpy(pages_array[1].lines[5].text, getWioTerminalID());
  strcpy(pages_array[2].lines[5].text, getWioTerminalID());
  updateLine(1, 5, ONLY_VALUE);
  updateLine(2, 5, ONLY_VALUE);
}

/********************************************************************************************
*** Loop Function
********************************************************************************************/

/**
 * @brief Main Funktion \n
 * Wird immer wieder ausgeführt. Auch ohne einer Endlosschleife wie @p while(1) @emoji :wink:
 *
 */
void loop()
{
  SAMCrashMonitor::iAmAlive();
  runLedHandler();
  networkConnectionHandler(&wio_Wifi, &wio_MQTT);            // rebuilds the network connection if necessary, updates the connection status
  currentPage = buttonHandler(currentPage);                  // you can change the page with the wio- buttons if you want
  displayHandler();                                          // refreshes the connection state on display
  currentPage = userFunctionsHandler(currentPage, &wio_MQTT, pages_array); // add your code in this function
}
