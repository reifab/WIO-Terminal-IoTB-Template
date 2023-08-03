/**
 * @file userFunctions.cpp
 * @author Fabian Reifler
 * @brief Funktionen vom Anwender
 * Du kannst deinen Code zwischen <tt>"START USER CODE"</tt> und <tt>"END USER CODE"</tt> einfügen
 * @version 0.1
 * @date 17.05.2023
 *
 * @copyright Copyright (c) 2023
 */

/********************************************************************************************
*** Includes
********************************************************************************************/
#include <Arduino.h>
#include "userFunctions.h"
#include "pages.h"
#include "wio_mqtt.h"
#include "display.h"

// START USER CODE: Includes

// END USER CODE: Includes

/********************************************************************************************
*** Defines
********************************************************************************************/

// START USER CODE: Defines

// END USER CODE: Defines

/********************************************************************************************
*** Functionprototypes
********************************************************************************************/
void onMqttMessage(int messageSize);
// START USER CODE: Functionprototypes

// END USER CODE: Functionprototypes

/********************************************************************************************
*** Module Global Parameters
********************************************************************************************/
// MQTT parameters
char topicList[][TOPIC_LENGTH] = ///< Liste der MQTT Topics, die abboniert werden sollen. Die Topics werden als String eingegeben. Das Letzte Zeichen sollte ein '\0' sein.
                                 ///< Die einzelnen Strings werden mit einem Komma getrennt (Siehe Beispiel. Coming soon...)
                                 ///< @attention Maximale 50 Zeichen pro Topic!
    {
        // START USER CODE: Subscribed Topics
        "KU291/Haus20/2OG/Lampe1\0", // Beispiel 1
        "KU291/Haus20/2OG/Lampe2\0"  // Beispiel 2
        // END USER CODE: Subscribed Topics
};


// START USER CODE: Global Variables

// END USER CODE: Global Variables

/********************************************************************************************
*** Objects
********************************************************************************************/
static wio_mqtt *wio_MQTT;
// START USER CODE: Objects

// END USER CODE: Objects

int initUserFunctions(wio_mqtt *ptr_wio_MQTT)
{
   // START USER CODE: Setup
  int currentPage = 0; // set currentPage


  // END USER CODE: Setup

  // MQTT Configuration
  wio_MQTT = ptr_wio_MQTT;
  wio_MQTT->addSubscribeList(topicList[0], sizeof(topicList)); // subscribe to all topics in topicList
  wio_MQTT->initMQTT(onMqttMessage);                           // init MQTT with callback function

  return currentPage;
}

void userFunctionsHandler(int currentPage, wio_mqtt *wio_MQTT, page_t pages_array[])
{
  const long userDefinedIntervall = 1000; // Interval time for user defined perdiodic task
  static long previousMillis;             // for user
  long currentMillis = millis();          // save millis

  // START USER CODE: variables
 
  // END USER CODE: variables

  if ((currentMillis - previousMillis >= userDefinedIntervall) || previousMillis == 0)
  {
    previousMillis = currentMillis; // refresh previousMillis
    // START USER CODE: user periodic task
    
    // END USER CODE: user periodic task
  }

  //================================//
  // Page dependet tasks            //
  //================================//
  switch (currentPage)
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
}

/********************************************************************************************
*** Functions
********************************************************************************************/

/**
 * @brief MQTT Callback Function for subscibed topics.
 * Diese Funktion wird automatisch aufgerufen, wenn ein abboniertes MQTT Topic eintrifft.\n
 * Im String <tt>@b payload_str</tt> steht der Inhalt des Topics.
 *
 *
 * @param messageSize  Länge der Nachricht
 */
void onMqttMessage(int messageSize)
{
  int elem = 0;
  wio_MQTT->setSubscribeState(true); // set subscribe state (for display)
  const char *topic = wio_MQTT->getMessageTopic();
  const char *payload = wio_MQTT->getMessagePayload();

  Serial.println("Message received:"); // print infos to SerialPort
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(payload);

  while (strcmp(topic, topicList[elem])) // search topic in topicList
  {
    elem++; // if not found, increment elem
  }

  switch (elem)
  {
  case 0:
    // START USER CODE: first element of the topicList
    
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

// START USER CODE: user functions
void demoFunction(void){
  Serial.println("Demo- Funktion wurde aufgerufen!");
}



// END USER CODE: user functions
