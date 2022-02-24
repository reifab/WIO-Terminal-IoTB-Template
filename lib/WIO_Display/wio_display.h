/**
 * @file wio_display.h
 * @author Beat Sturzenegger
 * @brief IoTB Bibliothek für den WIO Terminal Display.
 * @version 1.3
 * @date 18.01.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef WIO_DISPLAY_H
#define WIO_DISPLAY_H
#include "pages.h"

/********************************************************************************************
*** Defines
********************************************************************************************/
#define LINE_START_X  10    ///< Start position of the first line; x-coordinate
#define LINE_START_Y  55    ///< Start position of the first line; y-coordinate
#define LINE_VALUE_X  180   ///< Start position of the first line value; x-coordinate

/********************************************************************************************
*** Datatypes
********************************************************************************************/
/// Einstellungsmöglichkeiten für die @ref wio_display::updateLine(page_t, unsigned int, draw_setting_e) Funktion
typedef enum{
  FULL_LINE,    ///< Zeichnet die komplette Linie neu --> für @ref wio_display::updateLine
  ONLY_VALUE,   ///< Zeichnet nur den Zeilenwert neu --> für @ref wio_display::updateLine
}draw_setting_e;

/********************************************************************************************
*** Extern Variables
********************************************************************************************/
extern char log_text[15][50];

/********************************************************************************************
*** Interface description
********************************************************************************************/
class wio_display
{
  public:
    wio_display(int *mqtt, bool *pub, bool *sub, int *wlan, int *st, int *ch);    ///< Konstructor
    void initDisplay();                                                           ///< Display initialisieren
    void drawPage(page_t p);                                                      ///< Seite zeichnen
    void updateContext(page_t p);                                                 ///< Seiten inhalt zeichnen
    void updateLine(page_t p, unsigned int line_nr, draw_setting_e setting);      ///< Zeile zeichnen
    void updateInterfaceStatus();                                                 ///< Interface Icons updaten
    void loadingScreen(int);                                                      ///< Loading Screen aktivieren/deaktivieren
    void addLogText(const char * log_, bool append);                                    ///< Log Text hinzufügen
    
  private:
    void drawHeader(char *title, int sd_card_status, int mqtt_status, int wlan_status, int wlan_strength, int wlan_channel);
    void drawPageLine(line_t l, unsigned int line_nr, draw_setting_e setting);
    void drawIcons(int mqtt_status, bool mqtt_pub, bool mqtt_sub, int wlan_status, int wlan_strength, int wlan_channel, bool forced);
    void symbol_5(int offset);
    void symbol_2(int offset);
    void symbol_4(int offset);
    void symbol_9(int offset);
    void symbol_G(int offset);
    void symbol_H(int offset);
    void symbol_z(int offset);
    void symbol_dot(int offset);
};
#endif