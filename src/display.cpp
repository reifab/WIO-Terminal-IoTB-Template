/**
 * @file display.cpp
 * @author Fabian Reifler
 * @brief Anwendung des Displays auf dem Wio- Terminal.
 * @version 0.1
 * @date 2023-05-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "wio_display.h"
#include "networkConnection.h"
#include "display.h"
#include "pages.h"
#include "wio_wifi.h"
#include <Arduino.h>

/********************************************************************************************
*** Global Parameters
********************************************************************************************/
// Display Parameter
extern page_t pages_array[]; ///< extern Page Array, is coded in pages.c

// intervals for periodic tasks
const long interfaceIconIntervall = 2000; ///< Interval time for icons refreshing

/********************************************************************************************
*** Objects
********************************************************************************************/
static wio_display wio_disp = wio_display(NULL);

/**
 * @brief Initialisiert das Display mit dem Verbindungsstatus (MQTT und WLAN).
 *
 * @param conState Verbindungsstatus vom WLAN und MQTT Broker
 */
void initDisplay()
{
    wio_disp = wio_display(getConnectionStatePtr());
    wio_disp.initDisplay(); // init built-in display
}

/**
 * @brief Schaltet den Loading- Screen ein
 *
 */
void enableLoadingScreen()
{
    wio_disp.loadingScreen(1); // enable loading screen
}

/**
 * @brief Schaltet den Loading- Screen aus
 *
 */
void disableLoadingScreen()
{
    wio_disp.loadingScreen(0); // disable loading screen
}

/**
 * @brief Zeichnet die angegebene Menu- Seite (page) neu.
 *
 * @param page_array Array mit Informationen zur Seite (in pages.c)
 * @param currentPage Aktuelle Seite z.B. 1, 2, 3...
 */
void drawPage(page_t page_array[], int currentPage)
{
    wio_disp.drawPage(pages_array[currentPage]); // draw Page No. 1 (= currentPage)
}

/**
 * @brief Prüft zyklisch den WLAN status, verbindet neu falls nötig.\n
 * Aktuallisiert die Informatinen auf dem Display.
 *
 * @param wio_Wifi Zeiger auf das wio_wifi Objekt
 * @param connectionState Zeiger auf das connectionState- Objekt (Verbindungsstatus)
 */
void displayHandler()
{
    static long previousMillis;
    long currentMillis = millis();

    if ((currentMillis - previousMillis >= interfaceIconIntervall) || previousMillis == 0)
    {
        previousMillis = currentMillis; // refresh previousMillis
        wio_disp.updateInterfaceStatus(); // update Interface status on the display
    }
}

void updateLine(uint16_t myPage, int16_t myLine, draw_setting_e drawSetting)
{
    wio_disp.updateLine(pages_array[myPage], myLine, drawSetting);
}

/**
 * @brief Diese Funktion fügt einen Text zum Log Text hinzu
 *
 * @param log_ Text der hinzugefügt werden soll
 * @param append Soll der Text angehängt ( @p true ) oder auf eine neue Zeile geschrieben ( @p false ) werden
 */
void addLogText(const char *text, bool append)
{
    wio_disp.addLogText(text, append);
}