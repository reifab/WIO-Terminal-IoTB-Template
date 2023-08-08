/**
 * @file buttons.cpp
 * @author Fabian Reifler
 * @brief Es lassen sich hiermit alle Buttons vom Wio- Terminal initialisieren.\n 
 * Zusätzlich pollt der buttonHandler alle Buttons. Die Aktionen können vom Benutzer\n 
 * bestimmt werden.
 * @version 0.1
 * @date 2023-05-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "buttons.h"
#include "userFunctions.h"

/**
 * @brief Initialisiert die Buttons vom Wio- Terminal
 */
void initButtons(void)
{
    pinMode(WIO_KEY_A, INPUT_PULLUP);    // config button <A>
    pinMode(WIO_KEY_B, INPUT_PULLUP);    // config button <B>
    pinMode(WIO_KEY_C, INPUT_PULLUP);    // config button <C>
    pinMode(WIO_5S_UP, INPUT_PULLUP);    // config 5-way-switch <UP>
    pinMode(WIO_5S_DOWN, INPUT_PULLUP);  // config 5-way-switch <DOWN>
    pinMode(WIO_5S_LEFT, INPUT_PULLUP);  // config 5-way-switch <LEFT>
    pinMode(WIO_5S_RIGHT, INPUT_PULLUP); // config 5-way-switch <RIGHT>
    pinMode(WIO_5S_PRESS, INPUT_PULLUP); // config 5-way-switch <PRESS>
}

/**
 * @brief Pollt die Buttons vom Wio- Terminal und führt gewünschte Aktionen aus.
 * @param currentPage aktuelle Seite z.B. 1, 2, 3
 * @return uint16_t aktuelle Seite, die mit den Buttons auf Wunsch geaendert werden kann.
 *
 */
uint16_t buttonHandler(uint16_t currentPage)
{
    //================================//
    // Button Handler                 //
    //================================//
    if (digitalRead(WIO_5S_DOWN) == LOW) // 5-Way-Switch slided Down
    {
        delay(100);
        switch (currentPage)
        {
        case 1:
            // START USER CODE: PAGE 1
            
            // END USER CODE: PAGE 1
            break;

        default:
            break;
        }
    }
    else if (digitalRead(WIO_5S_UP) == LOW) // 5-Way-Switch slided Up
    {
        delay(100);
        switch (currentPage)
        {
        case 1:
            // START USER CODE: PAGE 1

            // END USER CODE: PAGE 1
            break;

        default:
            break;
        }
    }
    else if (digitalRead(WIO_5S_PRESS) == LOW) // 5-Way-Switch pressed
    {
        delay(100);
        switch (currentPage)
        {
        case 1:
            // START USER CODE: PAGE 1

            // END USER CODE: PAGE 1
            break;

        default:
            break;
        }
    }
    else if (digitalRead(WIO_5S_RIGHT) == LOW) // 5-Way-Switch slided to Right
    {
        delay(100);
        switch (currentPage)
        {
        case 1:
            // START USER CODE: PAGE 1

            // END USER CODE: PAGE 1
            break;

        default:
            break;
        }
    }
    else if (digitalRead(WIO_5S_LEFT) == LOW) // 5-Way-Switch slided to Left
    {
        delay(100);
        switch (currentPage)
        {
        case 1:
            // START USER CODE: PAGE 1

            // END USER CODE: PAGE 1
            break;

        default:
            break;
        }
    }
    else if (digitalRead(WIO_KEY_A) == LOW) // Button A pressed
    {
        delay(100);
        switch (currentPage)
        {
        case 1:
            // START USER CODE: PAGE 1

            // END USER CODE: PAGE 1
        default:
            break;
        }
    }
    else if (digitalRead(WIO_KEY_B) == LOW) // Button B pressed
    {
        delay(100);
        switch (currentPage)
        {
        case 1:
            // START USER CODE: PAGE 1

            // END USER CODE: PAGE 1
        default:
            break;
        }
    }
    else if (digitalRead(WIO_KEY_C) == LOW) // Button C pressed
    {
        delay(100);
        switch (currentPage)
        {
        case 1:
            // START USER CODE: PAGE 1

            // END USER CODE: PAGE 1
        default:
            break;
        }
    }
    return currentPage;
}