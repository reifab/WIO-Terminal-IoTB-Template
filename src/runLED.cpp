/**
 * @file runLED.cpp
 * @author Fabian Reifler
 * @brief Blinken der blauen LED neben USB Stecker (Lebenszeichen)
 * @version 0.1
 * @date 24.5.2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "runLED.h"
#include <Arduino.h>

/**
 * @brief Initialisiert die LED
 * 
 */
void initRunLed(void){
    pinMode(LED_BUILTIN, OUTPUT);
}

/**
 * @brief Lässt die LED im gewünschten Intervall blinken
 *
 */
void runLedHandler(void)
{
    const long runLED_Intervall = 250;     // Interval time for user defined perdiodic task
    static long previousMillis;             // for user
    long currentMillis = millis();          // save millis

    if ((currentMillis - previousMillis >= runLED_Intervall) || previousMillis == 0)
    {
        previousMillis = currentMillis; // refresh previousMillis
        static bool ledState = false;
        digitalWrite(LED_BUILTIN, ledState);
        if(ledState){
            ledState = false;
        }else{
            ledState = true;
        }
    }
}