/**
 * @file buttons.h
 * @author Fabian Reifler
 * @brief Unterstützung der Buttons auf dem Wio- Terminal.
 * Beispiel mit Pollen der Tasten.
 * @version 0.1
 * @date 2023-05-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef _BUTTONS_H_
#define _BUTTONS_H_

#include <Arduino.h>

/********************************************************************************************
*** Interface description
********************************************************************************************/
void initButtons(void); ///< Initialisiert die Buttons vom Wio- Terminal
uint16_t buttonHandler(uint16_t currentPage); ///< Pollt die buttons und führt gewünschte Aktionen aus.

#endif //_BUTTONS_H_
