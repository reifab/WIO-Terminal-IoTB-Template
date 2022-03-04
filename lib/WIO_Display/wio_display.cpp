/**
 * @file wio_display.cpp
 * @author Beat Sturzenegger
 * @brief IoTB Bibliothek für den WIO Terminal Display. \n
 * Diese Bibliothek bildet ein Grundgerüst für eine einfachere Nutzung des Displays. 
 * Mit dem File page.c können merhere Seiten vordefiniert werden. Diese Bibliothek ist dafür
 * zuständig, dass die Seiten gezeichnet, aktualisiert und verändert werden können. \n
 * Zusätzlich werden gewisse Interface Zustände angezeigt, sofern diese Verwendet werden.
 * zu den Interface gehören den WiFi, MQTT und SD Karte. \n 
 * @todo Auf einer Page die Interface Zustände genauer beschreiben. Coming soon...
 * @version 1.5
 * @date 14.02.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/********************************************************************************************
*** Includes
********************************************************************************************/
#include "wio_display.h"          // own display library
#include "TFT_eSPI.h"             // extern display library
#include "Seeed_FS.h"             // SD card library
#include "RawImage.h"             // image processing library
#include "Free_Fonts.h"           // free font library
#include "pages.h"                // page definition library
#include <stdint.h>               // integer type library

/********************************************************************************************
*** Objects
********************************************************************************************/
TFT_eSPI tft;   ///< Display Object

/********************************************************************************************
*** Variables
********************************************************************************************/
static int sd_card_status = 0;        // sd card status (0: no SD card, 1: SD card pluged in)
static int *mqtt_status_ptr = NULL;   // pointer to MQTT status
static bool *mqtt_pub_ptr = NULL;     // pointer to MQTT publish status
static bool *mqtt_sub_ptr = NULL;     // pointer to MQTT subscribe status
static int *wlan_status_ptr = NULL;   // pointer to WLAN status
static int *wlan_strength_ptr = NULL; // pointer to WLAN strength
static int *wlan_channel_ptr = NULL;  // pointer to WLAN channel
static int loading_screen_status = 0; // loading screen status
static const uint16_t NO_SD_CARD_IMG[] = {  // No sd card image
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0010 (16)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0020 (32)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0030 (48)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0040 (64)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0050 (80)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf4b2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xeb2c, 0xad14, 0xad55, 0xa514, 0xad55, 0xa514, // 0x0060 (96)
0xad55, 0xa534, 0xa534, 0xa534, 0xa534, 0xad55, 0xa534, 0xad55, 0xa514, 0xad55, 0xa514, 0xad55, 0xa534, 0xb514, 0xe9c7, 0xe8e3, // 0x0070 (112)
0xe8e3, 0xe8e3, 0xe904, 0xfefb, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffbe, 0xf28a, 0xe8e3, // 0x0080 (128)
0xe8e3, 0xe8e3, 0xe8e3, 0xb082, 0x0020, 0x0000, 0x0020, 0x0000, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, // 0x0090 (144)
0x0000, 0x0020, 0x0000, 0x0020, 0x0000, 0xc8a2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xf596, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x00a0 (160)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xfeba, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0x7041, 0x0020, 0x0000, 0x0020, // 0x00b0 (176)
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x0020, 0x0000, 0x0020, 0x0000, 0x9882, 0xe8e3, 0xe8e3, 0xe8e3, // 0x00c0 (192)
0xe8e3, 0xe269, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xec51, // 0x00d0 (208)
0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xd945, 0xad34, 0x6b6d, 0x0000, 0x4a49, 0xa534, 0x6b4d, 0x0000, 0x4a49, 0xa534, 0x6b6d, 0x0020, // 0x00e0 (224)
0x4a49, 0xa534, 0x6b6d, 0x5820, 0xe904, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xb4b2, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x00f0 (240)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xd6ba, 0x5165, 0xe0c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xf596, 0xa534, 0x0000, // 0x0100 (256)
0x73ae, 0xffdf, 0xa514, 0x0000, 0x7bcf, 0xffff, 0xa534, 0x0000, 0x73ae, 0xffff, 0xa514, 0xd0c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, // 0x0110 (272)
0xa082, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xf7be, 0x2965, 0x0000, // 0x0120 (288)
0x8061, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe945, 0xbcb2, 0x0000, 0x73ae, 0xffdf, 0xa514, 0x0000, 0x7bcf, 0xffdf, 0xad55, 0x0000, // 0x0130 (304)
0x73ae, 0xffdf, 0xcbef, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xd0c3, 0x0800, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0140 (320)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, 0x4a49, 0xed14, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe249, 0x2820, // 0x0150 (336)
0x73ae, 0xffdf, 0xa514, 0x0000, 0x7bcf, 0xffdf, 0xa534, 0x0000, 0x73ae, 0xff5d, 0xe945, 0xe8e3, 0xe8e3, 0xe8e3, 0xe9a6, 0x5020, // 0x0160 (352)
0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, // 0x0170 (368)
0x73ae, 0xffbe, 0xe249, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xb082, 0x73ae, 0xffdf, 0xa514, 0x0000, 0x7bcf, 0xffdf, 0xa534, 0x0000, // 0x0180 (384)
0x7b8e, 0xf3ef, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xc471, 0x0000, 0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0190 (400)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, 0x7bcf, 0xffdf, 0xbcb2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, // 0x01a0 (416)
0x9b4d, 0xffff, 0xa534, 0x0000, 0x7bcf, 0xffdf, 0xad55, 0x0000, 0xbaaa, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xf514, 0xa534, 0x0000, // 0x01b0 (432)
0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0020, 0x0000, // 0x01c0 (448)
0x31a6, 0x73ae, 0x4a69, 0xb0a2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xd904, 0x7bae, 0x4a49, 0x0000, 0x31a6, 0x7bcf, 0x4a49, 0x7020, // 0x01d0 (464)
0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe104, 0x83ae, 0x4a69, 0x0000, 0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x01e0 (480)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2800, 0xd8c3, 0xe8e3, 0xe8e3, 0xe8e3, // 0x01f0 (496)
0xe8e3, 0xb082, 0x0000, 0x0000, 0x0000, 0x0000, 0x2800, 0xd8c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0x7841, 0x0000, 0x0000, 0x0000, // 0x0200 (512)
0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xce59, 0x0000, 0x0000, // 0x0210 (528)
0x0000, 0x0000, 0x0000, 0x0020, 0x6841, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0x7841, 0x0000, 0x0000, 0x0000, 0xb082, 0xe8e3, // 0x0220 (544)
0xe8e3, 0xe8e3, 0xe8e3, 0xb082, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0230 (560)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0xa882, 0xe8e3, 0xe8e3, // 0x0240 (576)
0xe8e3, 0xe8e3, 0xd8c3, 0x2820, 0x0000, 0x7861, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xd0c3, 0x2020, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0250 (592)
0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xce59, 0x0020, 0x0000, // 0x0260 (608)
0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x1820, 0xd0a2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xb8a2, 0x3820, 0xe0c3, 0xe8e3, 0xe8e3, // 0x0270 (624)
0xe8e3, 0xe8c3, 0x6041, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0280 (640)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x0020, 0x0000, 0x6041, 0xe8c3, // 0x0290 (656)
0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xd0a2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0x9882, 0x0000, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, // 0x02a0 (672)
0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0020, 0x0000, // 0x02b0 (688)
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0xa082, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, // 0x02c0 (704)
0xc8a2, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x02d0 (720)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x02e0 (736)
0xc8a2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe0c3, 0x4820, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x02f0 (752)
0x0000, 0xa514, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, // 0x0300 (768)
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x5020, 0xe0c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0x8861, // 0x0310 (784)
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0320 (800)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0330 (816)
0x4820, 0xe0c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xb082, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0340 (832)
0x0000, 0xa514, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, // 0x0350 (848)
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xc8a2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, // 0x0360 (864)
0x7841, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xa514, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0370 (880)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xce59, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x0000, 0x0000, 0xa082, // 0x0380 (896)
0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe0c3, 0x3800, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0390 (912)
0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, // 0x03a0 (928)
0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x6041, 0xe8c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xd8c3, 0xc8a2, 0xe8e3, 0xe8e3, 0xe8e3, // 0x03b0 (944)
0xe8e3, 0xc0a2, 0x0000, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x03c0 (960)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xce59, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x2820, 0xd8c3, 0xe8e3, // 0x03d0 (976)
0xe8e3, 0xe8e3, 0xe8e3, 0x7041, 0x4820, 0xe0c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0x8861, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0x03e0 (992)
0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0020, // 0x03f0 (1008)
0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0xb082, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xa882, 0x0000, 0x0000, 0x9061, 0xe8e3, 0xe8e3, // 0x0400 (1024)
0xe8e3, 0xe8e3, 0xe0c3, 0x4820, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0410 (1040)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xce59, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8041, 0xe8e3, 0xe8e3, 0xe8e3, // 0x0420 (1056)
0xe8e3, 0xd8c3, 0x2000, 0x0000, 0x0000, 0x0000, 0xc8a2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xc8a2, 0x0000, 0x0000, 0x0000, 0x0000, // 0x0430 (1072)
0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, // 0x0440 (1088)
0x0000, 0x0000, 0x0000, 0x4020, 0xe0c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0x6041, 0x0000, 0x0000, 0x0000, 0x0000, 0x5020, 0xe0c3, // 0x0450 (1104)
0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0x9861, 0x0000, 0x0000, 0x0000, 0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0460 (1120)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xc0a2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, // 0x0470 (1136)
0xb28a, 0x4228, 0x73ae, 0x630c, 0x0000, 0x0000, 0x0000, 0x9061, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8c3, 0x5020, 0x0000, 0x0000, // 0x0480 (1152)
0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, // 0x0490 (1168)
0x0000, 0x0000, 0x9061, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe34d, 0xa514, 0x9492, 0xd69a, 0x94b2, 0x8c51, 0x9492, 0x73ae, 0x0000, // 0x04a0 (1184)
0xc8a2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xd0c3, 0x0800, 0x0000, 0x0000, 0xa514, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x04b0 (1200)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, 0x0000, 0x5820, 0xe8c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe924, 0x5841, // 0x04c0 (1216)
0x0000, 0x0000, 0x0000, 0x94b2, 0xf79e, 0xffdf, 0xf7be, 0x52aa, 0x5020, 0xe0c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xa082, 0x0000, // 0x04d0 (1232)
0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, // 0x04e0 (1248)
0x1000, 0xd0c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xf659, 0xad75, 0x4a69, 0x9cf3, 0xc638, 0xf7be, 0xffdf, 0xffdf, 0xef5d, 0x31a6, // 0x04f0 (1264)
0x0000, 0x9061, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8c3, 0x6041, 0x0000, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0500 (1280)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x0000, 0x0000, 0xa882, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xec71, 0xffdf, 0xce59, // 0x0510 (1296)
0xbdd7, 0xffff, 0xffdf, 0xffdf, 0xf7be, 0xc638, 0x528a, 0x0000, 0x0000, 0x0000, 0xc8a2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xd0c3, // 0x0520 (1312)
0x2000, 0xa514, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xce59, 0x0020, 0x7041, // 0x0530 (1328)
0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xea08, 0xc5b6, 0xad55, 0x39e7, 0xb596, 0xb5b6, 0xa534, 0x7bef, 0x2104, 0x0000, 0x0020, 0x0000, // 0x0540 (1344)
0x0020, 0x0000, 0x4820, 0xe0c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xa882, 0xa534, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0550 (1360)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xc638, 0x2800, 0xd8c3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0x9061, 0x0000, 0x0020, 0x0000, // 0x0560 (1376)
0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x0020, 0x0000, 0x9082, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, // 0x0570 (1392)
0xe8e3, 0xb4b2, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xce59, 0xb8a2, 0xe8e3, // 0x0580 (1408)
0xe8e3, 0xe8e3, 0xe8e3, 0xc0a2, 0x0000, 0x0020, 0x0000, 0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, // 0x0590 (1424)
0x0020, 0x0000, 0x0020, 0x0000, 0xc8a2, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe249, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x05a0 (1440)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xddb6, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe0c3, 0x4020, 0x0020, 0x0000, 0x0020, 0x0000, // 0x05b0 (1456)
0x0020, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x0020, 0x0000, 0x0020, 0x4820, 0xe0c3, 0xe8e3, 0xe8e3, // 0x05c0 (1472)
0xe8e3, 0xe8e3, 0xf596, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffbe, 0xf2cb, 0xe8e3, 0xe8e3, // 0x05d0 (1488)
0xe8e3, 0xe8e3, 0xbc92, 0xa534, 0xad55, 0xa534, 0xad55, 0xa534, 0xad55, 0xa534, 0xa534, 0xa534, 0xa534, 0xad55, 0xa534, 0xad55, // 0x05e0 (1504)
0xa534, 0xad55, 0xa534, 0xad55, 0xa534, 0xc471, 0xe8e3, 0xe8e3, 0xe8e3, 0xe8e3, 0xe904, 0xff1c, 0xffff, 0xffff, 0xffff, 0xffff, // 0x05f0 (1520)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0600 (1536)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0610 (1552)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0620 (1568)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x0630 (1584)
0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff // 0x0640 (1600)
};

/********************************************************************************************
*** Constructor
********************************************************************************************/
/**
 * @brief Konstruktor
 * 
 * @param mqtt Adresse der MQTT Status Variable
 * @param pub Adresse des MQTT Publish Zustandes Variable
 * @param sub Adresse des MQTT Subscribe Zustandes Variable
 * @param wlan Adresse der WLAN Status Variable
 * @param st Adresse der WLAN Stärke Variable
 * @param ch Adresse der WLAN Kannal Variable
 */
wio_display::wio_display(int *mqtt, bool *pub, bool *sub, int *wlan, int *st, int *ch)
{
  // save addresses
  mqtt_status_ptr = mqtt;
  mqtt_pub_ptr = pub;
  mqtt_sub_ptr = sub;
  wlan_status_ptr = wlan;
  wlan_strength_ptr = st;
  wlan_channel_ptr = ch;
}

/********************************************************************************************
*** Public Methodes
********************************************************************************************/
/**
 * @brief Methode um den Display zu initialisieren. Die Methode "startet" den TFT-Display, 
 * dreht den Display, dass sich die 3 Buttons Oben befinden und schaltet die 
 * Hintergrundbeleuchtung ein. Zusätzlich wird überprüft, ob eine SD Karte vorhanden ist.
 * 
 */
void wio_display::initDisplay()
{
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 16000000)) {
    sd_card_status = 0;   // no SD card
  }
  else
  {
    sd_card_status = 1;   // SD card in the slot
  }

  tft.begin();              // begin tft display
  tft.setRotation(3);       // set display rotation; 3: 5-way switch is on bottom, 1: 5-way switch is on top
  digitalWrite(LCD_BACKLIGHT, HIGH);    // turn on display backlight
}

/**
 * @brief Diese Methode zeichnet die komplette Seite.
 * @attention Wird diese Methode zu oft aufgerufen, flackert der Display.
 * 
 * @param p Page to draw
 */
void wio_display::drawPage(page_t p)
{
  int mqtt_s = *mqtt_status_ptr;
  int wlan_s = *wlan_status_ptr;
  int wlan_st = *wlan_strength_ptr;
  int wlan_ch = *wlan_channel_ptr;

  tft.fillScreen(TFT_BLACK);                                                // draw background
  drawHeader(p.title, sd_card_status, mqtt_s, wlan_s, wlan_st, wlan_ch);    // draw header
  for (int i = 0; i < NUMBERS_OF_LINES; i++)                                // for NUMBERS_OF_LINES times
  {
    drawPageLine(p.lines[i], i, FULL_LINE);                             // draw all the lines
  }
}

/**
 * @brief Diese Methode zeichnet den ganzen Seiten Inhalt (ohne Header, nur die Zeilen) neu.
 * 
 * @param p Seite (Page). Von der übergebenen Seite werden die darin beinhalteten Zeilen gezeichnet.
 * @attention Der Kopf der Seite wird nicht neu gezeichnet, dass bedeutet, das der Titel veraltet sein kann.
 */
void wio_display::updateContext(page_t p)
{
  // draw Lines
  for (int i = 0; i < NUMBERS_OF_LINES; i++)
  {
    drawPageLine(p.lines[i], i, ONLY_VALUE);
  }
}

/**
 * @brief Diese Methode Zeichnet eine Zeile neu. Mit dem Übergabeparameter setting, kann eingestellt werden, 
 * ob die ganze Zeile (Name und Wert) oder nur der Inhalt (Wert) neu gezeichnet wird.
 * @param p Seite (Page). Von der übergebenen Seite werden die darin beinhalteten Zeilen gezeichnet.
 * @param line_nr Die Zeilen Nummer, welche neu gezeichnet werden soll. \n Mögliche Parameter: 0 - @ref NUMBERS_OF_LINES -1
 * @param setting Eine Option, was alles neu gezeichnet werden soll. @ref draw_setting_e
 *  - @p FULL_LINE: Zeichnet die komplette Zeile neu
 *  - @p ONLY_VALUE: Zeichnet nur den Zeilenwert neu
 */
void wio_display::updateLine(page_t p, unsigned int line_nr, draw_setting_e setting)
{
  drawPageLine(p.lines[line_nr], line_nr, setting);
}

/**
 * @brief Diese Methode updatet die Icons oben rechts in der Ecke. Ist eine SD Karte vorhanden, werden 
 * Bilder von der SD Karte verwendet. Ist keine SD Karte vorhanden, zeichnet diese Methode
 * farbige Kreise.
 * @note Die Icons oder Kreise werden nur neu gezeichnet, wenn sich der Wert verändert hat.
 * Dadurch kann diese Methode oft aufgerufen werden, ohne dass die Icons flackern.
 * @todo Auf einer Page die Interface Zustände genauer beschreiben. Mit Bildern. Coming soon...
 * 
 */
void wio_display::updateInterfaceStatus()
{
  int mqtt_s = *mqtt_status_ptr;
  int wlan_s = *wlan_status_ptr;
  int wlan_st = *wlan_strength_ptr;
  int wlan_ch = *wlan_channel_ptr;
  bool mqtt_pub = *mqtt_pub_ptr;
  bool mqtt_sub = *mqtt_sub_ptr;

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE || cardType == 4)
  {
    sd_card_status = 0;
  }
  else
  {
    sd_card_status = 1;
  }
  drawIcons(mqtt_s, mqtt_pub, mqtt_sub, wlan_s, wlan_st, wlan_ch, false);   // draw Icons
}

/**
 * @brief Diese Funktion aktiviert den Loading Screen.
 * 
 * @param modus Loading Screen Zustand
 * - 0: Schaltet den Loading Screen aus
 * - 1: Schaltet den Loading Screen ein
 */
void wio_display::loadingScreen(int modus)
{
  if (modus == 1)
  {
    tft.fillScreen(TFT_BLACK);    // draw black background
    loading_screen_status = 1;    // set loadingscreen status
  }
  else
  {
    loading_screen_status = 0;    // set loadingscreen status
  }
}

/**
 * @brief Diese Methode fügt einen Text zum Log Text hinzu und zeichnet den Loading Screen neu
 * 
 * @param log_ Text der hinzugefügt werden soll
 * @param append Soll der Text angehängt ( @p true ) oder auf eine neue Zeile geschrieben ( @p false ) werden
 */
void wio_display::addLogText(const char *log_, bool append)
{
  static int line_cnt = 0;

  if(append)
  {
    strcat(log_text[line_cnt], log_);     // copy hand over text to log
  }
  else
  {
    if (line_cnt < 15)
    {
      strcpy(log_text[line_cnt], log_);   // copy hand over text to log
      line_cnt++;
    }
  }

  if(loading_screen_status)
  {
    tft.setFreeFont(FSS9);              // set font
    tft.setTextColor(TFT_DARKGREEN);    // set text color dark green for a hacker look

    for (int i = 0; i < 15; i++)
    {
      if ((String)log_text[i] != "")
      {
        tft.drawString((String)log_text[i], 5, 16 * i);   // draw log line for line
      }
    }
  }
}

/********************************************************************************************
*** Private Methodes
********************************************************************************************/
/**
 * @brief Diese Methode zeichnet den Kopf/Header der Seite. Der Kopf enthält Titel und die Interface Icons.
 * @attention Wird diese Methode zu oft aufgerufen, flackern die Icons.
 * 
 * @param title Titel der Seite
 * @param sd_card_status SD Karten Zustand
 * @param mqtt_status MQTT Verbindungszustand
 * @param wlan_status WLAN Verbindungszustand
 * @param wlan_strength WLAN Signalstärke
 * @param wlan_channel WLAN Kannal
 */
void wio_display::drawHeader(char *title, int sd_card_status, int mqtt_status, int wlan_status, int wlan_strength, int wlan_channel)
{
  // draw Header Background
  tft.fillRect(0, 0, 320, 40, TFT_WHITE);   // white Background
  tft.drawFastVLine(199, 0, 40, TFT_BLACK); // black Seperator

  // write Titel
  tft.setFreeFont(FSS18);         // set font
  tft.setTextColor(TFT_BLACK);    // set text color to black
  tft.drawString(title, 5, 5);    // draw text

  drawIcons(mqtt_status, false, false, wlan_status, wlan_strength, wlan_channel, true);   // draw icons
}

/**
 * @brief Diese Methode zeichnet eine Zeile.
 * 
 * @param l Zeile die neugezeichnet werden soll
 * @param line_nr Die Nummer der Zeile (für Positionsbestimmung auf dem Display) \n Mögliche Parameter: 0 - @ref NUMBERS_OF_LINES - 1
 * @param setting Eine Option, was alles neu gezeichnet werden soll. @ref draw_setting_e
 *  - @p FULL_LINE: Zeichnet die komplette Zeile neu
 *  - @p ONLY_VALUE: Zeichnet nur den Zeilenwert neu
 */
void wio_display::drawPageLine(line_t l, unsigned int line_nr, draw_setting_e setting)
{
  char buf[40];
  int value;
  int len;
  static int line_length[2][NUMBERS_OF_LINES] = { {0,0,0,0,0,0}, {0,0,0,0,0,0} };   // init value for 6 lines

  tft.setFreeFont(FSS9);                    // set Font
  tft.setTextColor(TFT_WHITE, TFT_BLACK);   // set Color White and Background Black

  if (setting == FULL_LINE)
  {
    len = tft.textWidth(l.line_name);   // measure length of the new string
    if(len < line_length[0][line_nr])   // compare new length with old length
    {
      tft.fillRect(LINE_START_X, LINE_START_Y + (line_nr * 30), line_length[0][line_nr], 18, TFT_BLACK);    // draw black rectangle to clear old stuff
    }
    tft.drawString(l.line_name, LINE_START_X, LINE_START_Y + (line_nr * 30)); // write line name
    line_length[0][line_nr] = len;                                            // refresh line length
  }

  // write line context, dependent of the line typ
  switch (l.line_typ)
  {
    case TEXT:
      if(tft.textWidth(l.text) <= 140)
      {
        len = tft.textWidth(l.text);        // measure length of the new string
        if(len < line_length[1][line_nr])   // compare new length with old length
        {
          tft.fillRect(LINE_VALUE_X, LINE_START_Y + (line_nr * 30), line_length[1][line_nr], 18, TFT_BLACK);  // draw black rectangle to clear old stuff
        }
        tft.drawString(l.text, LINE_VALUE_X, LINE_START_Y + (line_nr * 30));  // draw line text
        line_length[1][line_nr] = len;                                        // refresh line length
      }
      else
      {
        tft.fillRect(LINE_VALUE_X, LINE_START_Y + (line_nr * 30), 180, 18, TFT_BLACK);      // draw black rectangle to clear old stuff
        tft.setTextColor(TFT_RED, TFT_BLACK);                                               // set Color White and Background Black
        tft.drawString("Text too long!!!", LINE_VALUE_X, LINE_START_Y + (line_nr * 30));    // draws Error
        tft.setTextColor(TFT_WHITE, TFT_BLACK);                                             // set Color White and Background Black
        line_length[1][line_nr] = 0;                                                        // set line length to 0
      }
      break;

    case NUMERIC:
      // convert number to a string. The setting sets the number of decimal places
      switch (l.setting)
      {
        case DECIMAL_PLACES_0:
          sprintf(buf, "%.0f %s", l.value, l.text);
          break;
        case DECIMAL_PLACES_1:
          sprintf(buf, "%.1f %s", l.value, l.text);
          break;
        case DECIMAL_PLACES_2:
          sprintf(buf, "%.2f %s", l.value, l.text);
          break;
        case DECIMAL_PLACES_3:
          sprintf(buf, "%.3f %s", l.value, l.text);
          break;
        default:
          sprintf(buf, "%.2f %s", l.value, l.text);
      }
      if(tft.textWidth(buf) <= 140)
      {
        len = tft.textWidth(buf);           // measure length of the new string
        if(len < line_length[1][line_nr])   // compare new length with old length
        {
          tft.fillRect(LINE_VALUE_X, LINE_START_Y + (line_nr * 30), line_length[1][line_nr], 18, TFT_BLACK);    // draw black rectangle to clear old stuff
        }
        tft.drawString(buf, LINE_VALUE_X, LINE_START_Y + (line_nr * 30));   // draws value
        line_length[1][line_nr] = len;                                      // refresh line length
      }
      else
      {
        tft.fillRect(LINE_VALUE_X, LINE_START_Y + (line_nr * 30), 180, 18, TFT_BLACK);      // draw black rectangle to clear old stuff
        tft.setTextColor(TFT_BLACK, TFT_BLACK);                                             // set Color White and Background Black
        tft.drawString("Text too long!!!", LINE_VALUE_X, LINE_START_Y + (line_nr * 30));    // draws Error
        tft.setTextColor(TFT_WHITE, TFT_BLACK);                                             // set Color White and Background Black
        line_length[1][line_nr] = 0;                                                        // set line length to 0
      }
      break;

    case BAR:
      value = l.value;
      if (value > 100) value = 100;   // set upper limit
      if (value < 0)   value = 0;     // set under limit
      tft.fillRect(LINE_VALUE_X, LINE_START_Y + (line_nr * 30) - 2, 104, 20, TFT_WHITE);    // draw outer rectangle, color white
      tft.fillRect(LINE_VALUE_X + 2, LINE_START_Y + (line_nr * 30), value, 16, TFT_GREEN);  // draw inner rectangle, color: green
      switch (l.setting)
      {
        // draw the value in the bar
        case BAR_SHOW_VALUE:
          tft.setFreeFont(FSS9);              // set font
          tft.setTextColor(TFT_BLACK);        // set text color black
          sprintf(buf, "%d", (int)l.value);   // convert value to string
          tft.drawString(buf, LINE_VALUE_X + 42, LINE_START_Y + (line_nr * 30));  // draw value
          break;
      }
      break;

    case TIME:
    {
      int sek_ = (int)l.value % 100;            // extract seconds from vlaue
      int min_ = ((int)l.value % 10000) / 100;  // extract minutes from value
      int hou_ = (int)l.value / 10000;          // extract hour from value

      // convert value to a time format. The format is dependent of the setting
      switch (l.setting)
      {
        case TIME_HH_MM:
          sprintf(buf, "%d:%d", hou_, min_);            // convert time to string
          break;
        case TIME_HH_MM_SS:
          sprintf(buf, "%d:%d:%d", hou_, min_, sek_);   // convert time to string
          break;
          //        case TIME_DD_MM_YYYY:
          //          break;
          //        case TIME_HH_MM_DD_MM_YYYY:
          //          break;
        default:
          sprintf(buf, "%d:%d", hou_, min_);            // convert time to string
      }

      len = tft.textWidth(buf);           // measure length of the new string
      if(len < line_length[1][line_nr])   // compare new length with old length
      {
        tft.fillRect(LINE_VALUE_X, LINE_START_Y + (line_nr * 30), line_length[1][line_nr], 18, TFT_BLACK);    // draw black rectangle to clear old stuff
      }
      tft.drawString(buf, LINE_VALUE_X, LINE_START_Y + (line_nr * 30));   // draw value
      line_length[1][line_nr] = len;                                      // refresh line length

      }break;

    case END_LINE_TYP:
      Serial.println("The comment said not to use this!!!");
      break;
  }
}

/**
 * @brief Updated die Interface Icons.
 * @note Ist der Übergabeparameter <tt> forced = false </tt>, dann werden die Icons oder Kreise  nur neu gezeichnet, wenn sich der Übergabeparameter seit dem letzten Aufruf verändert hat.
 * Dadurch kann diese Methode oft aufgerufen werden, ohne dass die Icons flackern.
 * @todo Formattierung checking
 * @param mqtt_status MQTT Verbindungsstatus
 * @param mqtt_pub MQTT Publish Status
 * @param mqtt_sub MQTT Subscribe Status
 * @param wlan_status WLAN Verbindungsstatus
 * @param wlan_strength WLAN Verbindungsstärke
 * @param wlan_channel WLAN Kanal
 * @param forced Kann das Zeichnen erzwingen
 * - @p False: Die Icons werden nur neu gezeichnet, wenn sich der Wert vom vorherigen Aufruf unterscheidet
 * - @p True: Die Icons werden auf jedenfall neu gezeichnet
 */
void wio_display::drawIcons(int mqtt_status, bool mqtt_pub, bool mqtt_sub, int wlan_status, int wlan_strength, int wlan_channel, bool forced)
{
  static int old_wlan_status = -99;     // set default value
  static int old_wlan_strength = -99;   // set default value
  static int old_wlan_channel = -99;    // set default value
  static int old_mqtt_status = -99;     // set default value
  static bool old_mqtt_pub = -99;       // set default value
  static bool old_mqtt_sub = -99;       // set default value

  // draw SD Card Status
  if (sd_card_status)
  {
    drawImage<uint16_t>("sys/img/bmp/sd_card.bmp", 280, 0);   // draw image from sd card
  }
  else
  {
    tft.pushImage((int32_t)280, (int32_t)0, (int32_t)40, (int32_t)40, &NO_SD_CARD_IMG[0]);  // draw on chip safed image
  }

  // MQTT Status
  if ((old_mqtt_status != mqtt_status) || (old_mqtt_pub != mqtt_pub) || (old_mqtt_sub != mqtt_sub) || forced)   // has something changed or is draw forced
  {
    tft.fillRect(250, 0, 10, 10, TFT_WHITE); // draw white rectangle to clear old stuff
    
    if (mqtt_status)
    {   // connected to broker
      if (sd_card_status) {
        drawImage<uint16_t>("sys/img/bmp/MQTT_on.bmp", 240, 0);   // draw image from sd card
      } else {

        tft.fillCircle(240 + 20, 20, 10, TFT_GREEN);    // draw green circle
        tft.drawCircle(240 + 20, 20, 10, TFT_BLACK);    // draw circle border
      }
    }
    else
    {   // disconnected to broker
      if (sd_card_status)
        drawImage<uint16_t>("sys/img/bmp/MQTT_off.bmp", 240, 0);   // draw image from sd card
      else {

        tft.fillCircle(240 + 20, 20, 10, TFT_RED);      // draw red circle
        tft.drawCircle(240 + 20, 20, 10, TFT_BLACK);    // draw circle border
      }
    }
    
    if(mqtt_pub)
    {
      // draw publish arrow
      tft.drawPixel(250 + 0, 1, TFT_WHITE);
      tft.drawPixel(250 + 1, 1, TFT_WHITE);
      tft.drawPixel(250 + 2, 1, TFT_BLACK);
      
      tft.drawPixel(250 + 0, 2, TFT_WHITE);
      tft.drawPixel(250 + 1, 2, TFT_BLACK);
      tft.drawPixel(250 + 2, 2, TFT_BLACK);

      tft.drawPixel(250 + 0, 3, TFT_BLACK);
      tft.drawPixel(250 + 1, 3, TFT_WHITE);
      tft.drawPixel(250 + 2, 3, TFT_BLACK);

      tft.drawPixel(250 + 0, 4, TFT_WHITE);
      tft.drawPixel(250 + 1, 4, TFT_WHITE);
      tft.drawPixel(250 + 2, 4, TFT_BLACK);

      tft.drawPixel(250 + 0, 5, TFT_WHITE);
      tft.drawPixel(250 + 1, 5, TFT_WHITE);
      tft.drawPixel(250 + 2, 5, TFT_BLACK);

      tft.drawPixel(250 + 3, 1, TFT_WHITE);
      tft.drawPixel(250 + 3, 2, TFT_WHITE);
      tft.drawPixel(250 + 3, 3, TFT_WHITE);
      tft.drawPixel(250 + 3, 4, TFT_WHITE);
      tft.drawPixel(250 + 3, 5, TFT_WHITE);
    }

    if(mqtt_sub)
    {
      // draw subscribe arrow
      tft.drawPixel(254 + 0, 1, TFT_BLACK);
      tft.drawPixel(254 + 1, 1, TFT_WHITE);
      tft.drawPixel(254 + 2, 1, TFT_WHITE);
      
      tft.drawPixel(254 + 0, 2, TFT_BLACK);
      tft.drawPixel(254 + 1, 2, TFT_WHITE);
      tft.drawPixel(254 + 2, 2, TFT_WHITE);

      tft.drawPixel(254 + 0, 3, TFT_BLACK);
      tft.drawPixel(254 + 1, 3, TFT_WHITE);
      tft.drawPixel(254 + 2, 3, TFT_BLACK);

      tft.drawPixel(254 + 0, 4, TFT_BLACK);
      tft.drawPixel(254 + 1, 4, TFT_BLACK);
      tft.drawPixel(254 + 2, 4, TFT_WHITE);

      tft.drawPixel(254 + 0, 5, TFT_BLACK);
      tft.drawPixel(254 + 1, 5, TFT_WHITE);
      tft.drawPixel(254 + 2, 5, TFT_WHITE);

      tft.drawPixel(250 + 3, 1, TFT_WHITE);
      tft.drawPixel(250 + 3, 2, TFT_WHITE);
      tft.drawPixel(250 + 3, 3, TFT_WHITE);
      tft.drawPixel(250 + 3, 4, TFT_WHITE);
      tft.drawPixel(250 + 3, 5, TFT_WHITE);
    }

    old_mqtt_pub = mqtt_pub;        // overwrite old value
    old_mqtt_sub = mqtt_sub;        // overwrite old value
    old_mqtt_status = mqtt_status;  // overwrite old value
  }

  // WLAN Status
  // wlan strength
  if((old_wlan_strength != wlan_strength) || forced)   // has something changed or is draw forced
  {
    if (sd_card_status) {
      forced = true;        // set to forced, that the frequence band also will be redrawn
    }
    // really god connection
    if (wlan_strength < 0 && wlan_strength > -50)
    {
      if (sd_card_status) {
        drawImage<uint16_t>("sys/img/bmp/wlan_full.bmp", 200, 0);   // draw image from sd card
      } else {
        tft.fillCircle(200 + 20, 20, 10, TFT_GREEN);    // draw green circle
        tft.drawCircle(200 + 20, 20, 10, TFT_BLACK);    // draw circle border
      }
    }
    // good connection
    else if (wlan_strength <= -50 && wlan_strength >= -59)
    {
      if (sd_card_status)
      {
        drawImage<uint16_t>("sys/img/bmp/wlan_mid.bmp", 200, 0);    // draw image from sd card
      } else {
        tft.fillCircle(200 + 20, 20, 10, TFT_YELLOW);   // draw yellow circle
        tft.drawCircle(200 + 20, 20, 10, TFT_BLACK);    // draw circle border
      }
    }
    // bad connection
    else if (wlan_strength <= -60 && wlan_strength >= -69)
    {
      if (sd_card_status)
      {
        drawImage<uint16_t>("sys/img/bmp/wlan_low.bmp", 200, 0);    // draw image from sd card
      } else {
        tft.fillCircle(200 + 20, 20, 10, TFT_ORANGE);   // draw orange circle
        tft.drawCircle(200 + 20, 20, 10, TFT_BLACK);    // draw circle border
      }
    }

    // no connection
    else if (wlan_strength < -70)
    {
      if (sd_card_status) {
        drawImage<uint16_t>("sys/img/bmp/wlan_no.bmp", 200, 0);   // draw image from sd card
      } else {
        tft.fillCircle(200 + 20, 20, 10, TFT_RED);      // draw red circle
        tft.drawCircle(200 + 20, 20, 10, TFT_BLACK);    // draw circle border
      }
    }
    else
    {
      if (sd_card_status) {
        drawImage<uint16_t>("sys/img/bmp/wlan_no_red.bmp", 200, 0);   // draw image from sd card
      } else {
        tft.fillCircle(200 + 20, 20, 10, TFT_LIGHTGREY);  // draw grey circle
        tft.drawCircle(200 + 20, 20, 10, TFT_BLACK);      // draw circle border
      }
    }
    old_wlan_strength = wlan_strength;      // overwrite old value
  }

  // wlan channel
  if((old_wlan_channel != wlan_channel) || forced)  // has something changed or is draw forced
  {
    tft.fillRect(200, 0, 30, 8, TFT_WHITE);         // draw white rectangle to clear old Stuff
    
    if (wlan_channel >= 1 && wlan_channel <= 14)    // 2.4 GHz channels
    {
      symbol_2(0);
      symbol_dot(5);
      symbol_4(7);
      symbol_G(12);
      symbol_H(17);
      symbol_z(22);
    }
    else if (wlan_channel >= 32 && wlan_channel <= 68)  // 5 GHz channels
    {
      symbol_5(0);
      symbol_G(5);
      symbol_H(10);
      symbol_z(15);
    }
    else if (wlan_channel >= 96 && wlan_channel <= 165) // 5.9 GHz channels
    {
      symbol_5(0);
      symbol_dot(5);
      symbol_9(7);
      symbol_G(12);
      symbol_H(17);
      symbol_z(22);
    }
    old_wlan_channel = wlan_channel;    // overwrite old value
  }
}

/**
 * @brief Zeichet eine kleine '5' auf die oberen rechten Display Ecke
 * 
 * @param offset x-Offset
 */
void wio_display::symbol_5(int offset)
{
  tft.drawPixel(201 + offset, 1, TFT_BLACK);
  tft.drawPixel(202 + offset, 1, TFT_BLACK);
  tft.drawPixel(203 + offset, 1, TFT_BLACK);
  tft.drawPixel(204 + offset, 1, TFT_BLACK);

  tft.drawPixel(201 + offset, 2, TFT_BLACK);

  tft.drawPixel(201 + offset, 3, TFT_BLACK);
  tft.drawPixel(202 + offset, 3, TFT_BLACK);
  tft.drawPixel(203 + offset, 3, TFT_BLACK);

  tft.drawPixel(204 + offset, 4, TFT_BLACK);

  tft.drawPixel(201 + offset, 5, TFT_BLACK);
  tft.drawPixel(204 + offset, 5, TFT_BLACK);

  tft.drawPixel(202 + offset, 6, TFT_BLACK);
  tft.drawPixel(203 + offset, 6, TFT_BLACK);
}

/**
 * @brief Zeichet eine kleine '2' auf die oberen rechten Display Ecke
 * 
 * @param offset x-Offset
 */
void wio_display::symbol_2(int offset)
{
  tft.drawPixel(202 + offset, 1, TFT_BLACK);
  tft.drawPixel(203 + offset, 1, TFT_BLACK);

  tft.drawPixel(201 + offset, 2, TFT_BLACK);
  tft.drawPixel(204 + offset, 2, TFT_BLACK);

  tft.drawPixel(204 + offset, 3, TFT_BLACK);

  tft.drawPixel(203 + offset, 4, TFT_BLACK);

  tft.drawPixel(202 + offset, 5, TFT_BLACK);

  tft.drawPixel(201 + offset, 6, TFT_BLACK);
  tft.drawPixel(202 + offset, 6, TFT_BLACK);
  tft.drawPixel(203 + offset, 6, TFT_BLACK);
  tft.drawPixel(204 + offset, 6, TFT_BLACK);
}

/**
 * @brief Zeichet eine kleine '4' auf die oberen rechten Display Ecke
 * 
 * @param offset x-Offset
 */
void wio_display::symbol_4(int offset)
{
  tft.drawPixel(201 + offset, 1, TFT_BLACK);
  tft.drawPixel(203 + offset, 1, TFT_BLACK);

  tft.drawPixel(201 + offset, 2, TFT_BLACK);
  tft.drawPixel(203 + offset, 2, TFT_BLACK);

  tft.drawPixel(201 + offset, 3, TFT_BLACK);
  tft.drawPixel(203 + offset, 3, TFT_BLACK);

  tft.drawPixel(201 + offset, 4, TFT_BLACK);
  tft.drawPixel(202 + offset, 4, TFT_BLACK);
  tft.drawPixel(203 + offset, 4, TFT_BLACK);
  tft.drawPixel(204 + offset, 4, TFT_BLACK);

  tft.drawPixel(203 + offset, 5, TFT_BLACK);

  tft.drawPixel(203 + offset, 6, TFT_BLACK);
}

/**
 * @brief Zeichet eine kleine '9' auf die oberen rechten Display Ecke
 * 
 * @param offset x-Offset
 */
void wio_display::symbol_9(int offset)
{
  tft.drawPixel(202+offset, 1, TFT_BLACK);
  tft.drawPixel(203+offset, 1, TFT_BLACK);

  tft.drawPixel(201+offset, 2, TFT_BLACK);
  tft.drawPixel(204+offset, 2, TFT_BLACK);

  tft.drawPixel(201+offset, 3, TFT_BLACK);
  tft.drawPixel(204+offset, 3, TFT_BLACK);

  tft.drawPixel(202+offset, 4, TFT_BLACK);
  tft.drawPixel(203+offset, 4, TFT_BLACK);
  tft.drawPixel(204+offset, 4, TFT_BLACK);

  tft.drawPixel(204+offset, 5, TFT_BLACK);

  tft.drawPixel(202+offset, 6, TFT_BLACK);
  tft.drawPixel(203+offset, 6, TFT_BLACK);
}

/**
 * @brief Zeichet ein kleines 'G' auf die oberen rechten Display Ecke
 * 
 * @param offset x-Offset
 */
void wio_display::symbol_G(int offset)
{
  tft.drawPixel(202 + offset, 1, TFT_BLACK);
  tft.drawPixel(203 + offset, 1, TFT_BLACK);

  tft.drawPixel(201 + offset, 2, TFT_BLACK);
  tft.drawPixel(204 + offset, 2, TFT_BLACK);

  tft.drawPixel(201 + offset, 3, TFT_BLACK);

  tft.drawPixel(201 + offset, 4, TFT_BLACK);
  tft.drawPixel(203 + offset, 4, TFT_BLACK);
  tft.drawPixel(204 + offset, 4, TFT_BLACK);

  tft.drawPixel(201 + offset, 5, TFT_BLACK);
  tft.drawPixel(204 + offset, 5, TFT_BLACK);

  tft.drawPixel(202 + offset, 6, TFT_BLACK);
  tft.drawPixel(203 + offset, 6, TFT_BLACK);
}

/**
 * @brief Zeichet ein kleines 'H' auf die oberen rechten Display Ecke
 * 
 * @param offset x-Offset
 */
void wio_display::symbol_H(int offset)
{
  tft.drawPixel(201 + offset, 1, TFT_BLACK);
  tft.drawPixel(204 + offset, 1, TFT_BLACK);

  tft.drawPixel(201 + offset, 2, TFT_BLACK);
  tft.drawPixel(204 + offset, 2, TFT_BLACK);

  tft.drawPixel(201 + offset, 3, TFT_BLACK);
  tft.drawPixel(204 + offset, 3, TFT_BLACK);

  tft.drawPixel(201 + offset, 4, TFT_BLACK);
  tft.drawPixel(202 + offset, 4, TFT_BLACK);
  tft.drawPixel(203 + offset, 4, TFT_BLACK);
  tft.drawPixel(204 + offset, 4, TFT_BLACK);

  tft.drawPixel(201 + offset, 5, TFT_BLACK);
  tft.drawPixel(204 + offset, 5, TFT_BLACK);

  tft.drawPixel(201 + offset, 6, TFT_BLACK);
  tft.drawPixel(204 + offset, 6, TFT_BLACK);
}

/**
 * @brief Zeichet ein kleines 'z' auf die oberen rechten Display Ecke
 * 
 * @param offset x-Offset
 */
void wio_display::symbol_z(int offset)
{
  tft.drawPixel(201 + offset, 3, TFT_BLACK);
  tft.drawPixel(202 + offset, 3, TFT_BLACK);
  tft.drawPixel(203 + offset, 3, TFT_BLACK);
  tft.drawPixel(204 + offset, 3, TFT_BLACK);

  tft.drawPixel(202 + offset, 4, TFT_BLACK);

  tft.drawPixel(203 + offset, 5, TFT_BLACK);

  tft.drawPixel(201 + offset, 6, TFT_BLACK);
  tft.drawPixel(202 + offset, 6, TFT_BLACK);
  tft.drawPixel(203 + offset, 6, TFT_BLACK);
  tft.drawPixel(204 + offset, 6, TFT_BLACK);
}

/**
 * @brief Zeichet ein '.' auf die oberen rechten Display Ecke
 * 
 * @param offset x-Offset
 */
void wio_display::symbol_dot(int offset)
{
  tft.drawPixel(201 + offset, 6, TFT_BLACK);
}