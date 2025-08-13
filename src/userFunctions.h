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
void setMQTTUserAndPassword(const char *mqtt_user, const char *mqtt_password);
int initUserFunctions(wio_mqtt *wio_MQTT);
int userFunctionsHandler(int currentPage, wio_mqtt *wio_MQTT, page_t pages_array[]); 


// START USER CODE: Custom function prototypes

// END USER CODE: Custom function prototypes

struct PinCfg {
  int heater = -1;         // OUTPUT, optional
  int light = -1;          // OUTPUT, optional
  int light_rgb = -1;      // OUTPUT, optional
  int solar_analog = -1;   // ANALOG, optional
  int window_in = -1;      // INPUT, optional
};


#endif