/**
 * @file pages.c
 * @author Beat Sturzenegger
 * @brief 
 * @version 1.1
 * @date 14.02.2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/********************************************************************************************
*** Includes
********************************************************************************************/
#include "pages.h"

/********************************************************************************************
*** Variables
********************************************************************************************/
/// The pages can be preset here 
page_t pages_array[] = 
{
// Page 0
  {
    "Testseite",  
    { 
      //Name            | Typ     | Wert    | Textwert/Einheit  | Einstellung
      { "Textausgabe",    TEXT,     0,        "HELLO",            DEFAULT},           // Line 0
      { "Bargraph",       BAR,      50,       "",                 BAR_SHOW_VALUE},    // Line 1
      { "Zahlausgabe",    NUMERIC,  50,       "",                 DECIMAL_PLACES_1},  // Line 2
      { "Prozent",        NUMERIC,  24,       "%",                DEFAULT},           // Line 3
      { "Zeit",           TIME,     221645,   "",                 TIME_HH_MM},        // Line 4
      { "RSSI",           NUMERIC,  0,        "dB",               DEFAULT}            // Line 5
    }
  },
// Page 1
  {
    "TITEL",  
    { 
      //Name            | Typ     | Wert    | Textwert/Einheit  | Einstellung
      { "Zeile 1",        TEXT,     0,        "",                 DEFAULT},           // Line 0
      { "Zeile 2",        TEXT,     0,        "",                 DEFAULT},           // Line 1
      { "Zeile 3",        TEXT,     0,        "",                 DEFAULT},           // Line 2
      { "Zeile 4",        TEXT,     0,        "",                 DEFAULT},           // Line 3
      { "Zeile 5",        TEXT,     0,        "",                 DEFAULT},           // Line 4
      { "Zeile 6",        TEXT,     0,        "",                 DEFAULT}            // Line 5
    }
  },
// Page 2
  {
    "NULL"
  }
};
