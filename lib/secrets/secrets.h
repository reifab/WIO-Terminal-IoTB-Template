/**
 * @file secrets.h
 * @author Beat Sturzenegger / Fabian Reifler
 * @brief Zugangsdaten für die WiFi Verbindung und den MQTT Broker
 * @version 1.1
 * @date 08.03.2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef SECRETS_H
#define SECRETS_H

// WiFi data
const char *ssid = "";     ///< WLAN SSID
const char *password = ""; ///< WLAN Password

// MQTT data
const char *default_mqtt_broker = "172.20.1.51"; ///< MQTT Broker URL
const uint16_t default_mqtt_port = 1883;         ///< port for the Broker
const char *mqtt_user = "";                     ///< user for the Broker
const char *mqtt_password = "";        ///< password for the Broker
const char *mqtt_id = "";                       ///< not used yet

#endif
