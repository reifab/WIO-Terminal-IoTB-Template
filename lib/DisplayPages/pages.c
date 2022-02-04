/**
 * @file pages.c
 * @author Beat Sturzenegger
 * @brief 
 * @version 1.0
 * @date 18.01.2022
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
      { "Textausgabe",    TEXT,     0,        "HELLO",            0},               // Line 1
      { "Bargraph",       BAR,      50,       "",                 BAR_SHOW_VALUE},  // Line 2
      { "Zahlausgabe",    NUMERIC,  50,       "",                 1},               // Line 3
      { "Prozent",        NUMERIC,  24,       "%",                0},               // Line 4
      { "Zeit",           TIME,     221645,   "",                 TIME_HH_MM},      // Line 5
      { "RSSI",           NUMERIC,  0,        "dB",               0}                // Line 6
    }
  },
// Page 1
  {
    "TITEL",  
    { 
      //Name            | Typ     | Wert    | Textwert/Einheit  | Einstellung
      { "Zeile 1",        TEXT,     0,        "",                 0},               ///< Line 1
      { "Zeile 2",        TEXT,     0,        "",                 0},               ///< Line 2
      { "Zeile 3",        TEXT,     0,        "",                 0},               ///< Line 3
      { "Zeile 4",        TEXT,     0,        "",                 0},               ///< Line 4
      { "Zeile 5",        TEXT,     0,        "",                 0},               ///< Line 5
      { "Zeile 6",        TEXT,     0,        "",                 0}                ///< Line 6
    }
  },
// Page 2
  {
    "NULL"
  }
};
