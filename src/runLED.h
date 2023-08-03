/**
 * @file runLED.h
 * @author Fabian Reifler
 * @brief Blinken der blauen LED neben USB Stecker (Lebenszeichen)
 * @version 0.1
 * @date 24.5.2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

void initRunLed(void); ///< Initialisiert die LED
void runLedHandler(void); ///< Lässt die LED im gewünschten Intervall blinken
