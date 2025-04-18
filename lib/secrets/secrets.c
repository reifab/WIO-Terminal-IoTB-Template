#include "secrets.h"

/**
 * @file
 * @brief   Secrets-Template
 * 
 *          Create a c-file in the folder `src/secrets.local` where you declare 
 *          your local secrets. The folder is exempt from version control, so 
 *          your secrets are not accidentially published.
 *
 * @author  Ueli Niederer
 * @date    04.10.2023
 *
 * @copyright Copyright (c) 2023 GBS St.Gallen
 */

__attribute__((weak)) tSecretsWifi secrets_wifi = {
    .ssid = "default",
    .passkey = "default"
};

__attribute__((weak)) tSecretsMqtt secrets_mqtt = {
    .broker = "192.168.178.48",
    .clientId = "",
    .port = 1883,
    .user = "",
    .password = "",
};
