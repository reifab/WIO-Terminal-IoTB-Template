/**
 * @file pages.h
 * @author Beat Sturzenegger
 * @brief 
 * @version 1.1
 * @date 14.02.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef PAGES_H
#define PAGES_H
/********************************************************************************************
*** Defines
********************************************************************************************/
#define NUMBERS_OF_LINES  6   ///< Anzahl Zeilen pro Seite

/********************************************************************************************
*** Enumerations
********************************************************************************************/
/// Enum für die verschiedenen Zeilentypen
/** @todo Umrechnung von Value zu Zeit beschreiben */
typedef enum{
  TEXT,         ///< Zeigt einen Text an
  NUMERIC,      ///< Zeigt einen Zahlenwert an. Die Nachkommastellen kann mit @ref settings_e eingestellt werden. 
                ///< Kann auch eine Masseinheit anzeigen. Siehe @ref line_t
  BAR,          ///< Zeigt einen Balken an, wie ein Ladebalken oder Balkendiagramm
  TIME,         ///< Zeigt eine Zeit an. Das Zeitformat kann mit @ref settings_e eingestellt werden.

  END_LINE_TYP  ///< muss das letzte Element sein. Nicht verwenden!!!
}line_typ_e;

/// Enum für die verschiednenen Zeilen Einstellungen
typedef enum{
// General
  DEFAULT,            ///< Wird verwendet, wenn keine Einstellung möglich oder um Default zu verwenden
// Nummeric
  DECIMAL_PLACES_0,   ///< Zeigt 0 Nachkommastellen an --> für @ref line_typ_e @p NUMERIC   
  DECIMAL_PLACES_1,   ///< Zeigt 1 Nachkommastellen an --> für @ref line_typ_e @p NUMERIC   
  DECIMAL_PLACES_2,   ///< Zeigt 2 Nachkommastellen an --> für @ref line_typ_e @p NUMERIC, DEFAULT  
  DECIMAL_PLACES_3,   ///< Zeigt 3 Nachkommastellen an --> für @ref line_typ_e @p NUMERIC   
// Bar
  BAR_EMPTY,        ///< Zeigt ein Balken an, ohne einen Wert --> für @ref line_typ_e @p BAR, DEFAULT
  BAR_SHOW_VALUE,   ///< Zeigt ein Balken an, mit dem Balkenwert auf dem Balken --> für @ref line_typ_e @p BAR 

// Time
  TIME_HH_MM,             ///< Zeitformat hh:mm, DEFAULT --> für @ref line_typ_e @p TIME
  TIME_HH_MM_SS,          ///< Zeitformat hh:mm:ss --> für @ref line_typ_e @p TIME 
//  TIME_DD_MM_YYYY,        ///< Zeitformat DD.MM.YYYY --> für @ref line_typ_e @p TIME 
//  TIME_HH_MM_DD_MM_YYYY,  ///< Zeitformat hh:mm, DD.MM.YYYY --> für @ref line_typ_e @p TIME 

  END_SETTING       ///< muss das letzte Element sein. Nicht verwenden!!!
}settings_e;

/// Struktur einer Linie
typedef struct{
  char line_name[16];   ///< Name der Zeile, welcher auf der linken Displayseite angezeigt wird
  line_typ_e line_typ;  ///< Zeiletyp, siehe enum: @ref line_typ_e
  float value;          ///< Zahlenwert der Zeile, ist relevant für die Zeilentypen @p NUMERIC, @p BAR und @p TIME
  char text[20];        ///< Textwert der Zeile, wenn der Zeilentyp @p TEXT ist. Beim Zeilentyp @p NUMERIC wird der Textinhalt als Masseinheit hinzugefügt. \n<b> Maximal 20 Zeichen!</b>
  int setting;          ///< Spezifische Einstellung für die Zeile. Siehe @ref settings_e
}line_t;

/// Struktur einer Seite
typedef struct{
  char title[16];                 ///< Name der Seite, welche oben auf dem Display angezeigt wird. \n<b>Maximal 16 Zeichen!</b>
  line_t lines[NUMBERS_OF_LINES]; ///< Array von Zeilen
}page_t;

#endif