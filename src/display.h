/**
 * @file display.h
 * @author Fabian Reifler
 * @brief Anwendung des Displays auf dem Wio- Terminal.
 * @version 0.1
 * @date 24.05.2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#define NEWLINE false ///< Konstante für die Funktion addLogText
#define APPEND true ///< Konstante für die Funktion addLogText

#include "wio_wifi.h"
#include "wio_display.h"

void initDisplay(); ///< Initialisiert das Display
void displayHandler(); ///< Zeichnet die Icons und aktualisiert die Anzeige
void enableLoadingScreen(void); ///< Schaltet den Loading- Screen ein
void disableLoadingScreen(void); ///< Schaltet den Loading- Screen aus
void addLogText(const char *text, bool append); ///< Fügt einen Text zum Log hinzu
void drawPage(page_t page_array[], int currentPage); ///< Zeichnet die angegebene Menu- Seite (page) neu.
void updateLine(uint16_t myPage, int16_t myLine, draw_setting_e drawSetting); ///< Aktualisiert eine Zeile auf dem Display

#endif