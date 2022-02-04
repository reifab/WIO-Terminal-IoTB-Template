/**
 * @file secrets.h
 * @author Beat Sturzenegger
 * @brief Zugangsdaten für die WiFi Verbindung und den MQTT Broker
 * @version 1.0
 * @date 18.01.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef SECRETS_H
#define SECRETS_H

// WiFi data
  const char* ssid = "SSID";            ///< WLAN SSID
  const char* password =  "PASSWORD";   ///< WLAN Password

// MQTT data
  const char *mqtt_server = "test.mosquitto.org";   ///< MQTT Broker URL
  const char *mqtt_user = "";                       ///< not used yet
  const char *mqtt_password = "";                   ///< not used yet
  const char *mqtt_id = "";                         ///< not used yet // optional

#endif
