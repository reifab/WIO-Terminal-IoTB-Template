#pragma once

/**
 * @file
 * @brief   (Default) secrets for accessing WiFi and MQTT Broker
 *
 * @author  Ueli Niederer
 * @date    04.10.2023
 *
 * @copyright Copyright (c) 2023 GBS St.Gallen
 */

#include <stdint.h>

// WiFi data
typedef struct sSecretsWifi {
    const char *ssid;
    const char *passkey;
}tSecretsWifi;

extern tSecretsWifi secrets_wifi;

// MQTT data
typedef struct sSecretsMqtt {
    const char *broker;
    uint16_t    port;
    const char *clientId;
    const char *user;
    const char *password;
}tSecretsMqtt;

extern tSecretsMqtt secrets_mqtt;
