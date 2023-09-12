/**
 * @file userFunctions.h
 * @author Fabian Reifler
 * @brief Funktionen vom Anwender
 * Du kannst deinen Code zwischen <tt>"START USER CODE"</tt> und <tt>"END USER CODE"</tt> einfügen
 * @version 0.1
 * @date 17.05.2023
 *
 * @copyright Copyright (c) 2023
 */

#ifndef _USER_FUNCTIONS_H_
#define _USER_FUNCTIONS_H_

#include "wio_wifi.h"
#include "wio_mqtt.h"
#include "pages.h"

/********************************************************************************************
*** Functionprototypes
********************************************************************************************/
void setMQTTUserAndPassword(wio_mqtt *ptr_wio_MQTT, const char *mqtt_user, const char *mqtt_password);
int initUserFunctions(wio_mqtt *wio_MQTT);
int userFunctionsHandler(int currentPage, wio_mqtt *wio_MQTT, page_t pages_array[]); 
void onMqttMessage(int messageSize);


// START USER CODE: Custom function prototypes

void demoFunction(void); //delete me, just an example

// END USER CODE: Custom function prototypes


#endif