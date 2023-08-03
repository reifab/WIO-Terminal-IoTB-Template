/**
 * @file networkConnection.h
 * @author Fabian Reifler
 * @brief Verbindungsaufbau zum WLAN und MQTT Broker
 * @version 0.1
 * @date 24.04.2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _NETWORK_CONNECTION_H_
#define _NETWORK_CONNECTION_H_

#include "wio_wifi.h"
#include "wio_mqtt.h"
#include "display.h"

/********************************************************************************************
*** Functionprototypes
********************************************************************************************/
void networkConnectionHandler(wio_wifi *wio_Wifi, wio_mqtt *wio_MQTT);
connection_state_t * getConnectionStatePtr(void); ///< gibt den Pointer auf die connectionState Struktur zurück
int getMQTTStatus(void); ///< gibt den MQTT Verbinungsstatus zurück
bool getMQTTPubStatus(void); ///< gibt den MQTT Publish Status zurück
bool getMQTTSubStatus(void); ///< gibt den MQTT Subscribe Status zurück
int getWLANStatus(void); ///< gibt den WLAN Verbinungsstatus zurück
int getWLANStrength(void); ///< gibt die WLAN Empfangsstärke zurück
int getWLANChannel(void); ///< gibt den WLAN Kanal zurück

#endif