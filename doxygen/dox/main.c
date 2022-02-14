/**
 * @page page_main_cpp Aufbau main.cpp
 * @section setup_section Setup Funktion
 * Diese Funktion wird beim Start einmal aufgerufen. Im Template werden der Display, das WiFi-Module und die Taster 
 * initialisiert. Zudem wird bereits eine Verbindung zum WLAN Access Point und zum MQTT Broker gemacht. \n
 * Der Benutzer kann zwischen @code // START USER CODE: Setup @endcode und @code // END USER CODE: Setup @endcode
 * seinen Code hinzufügen. Der User kann hier die angeschlossenen Sensoren und Aktoren initialisieren. \n
 * Es wird auch empholen, um hier die erste Seite anzuzeigen. Dazu dient der folgende Code als Beispiel: \n
 * @code
 * currentPage = 0;                              // set currentPage
 * wio_disp.drawPage(pages_array[currentPage]);  // draw Page No. 0 (= currentPage)
 * @endcode
 * Mit diesem Code wird die aktuelle Seite gesetzt und die komplette Seite gezeichnet.
 * 
 * @section loop_section
 * Die loop-Funktion wird nach der setup-Funktion aufgerufen. Die loop-Funktion wird, auch ohne Endlosschleife, 
 * immer wieder ausgeführt. \n
 * Die Funktion ist in drei Bereiche unterteilt: @ref button_handler, @ref page_dependet_tasks und @ref periodic_tasks.
 * 
 * @subsection button_handler Button Handler
 * 
 * @subsection page_dependet_tasks Seitenabhängige Aufgaben
 * 
 * @subsection periodic_tasks Periodische Aufgaben
 * 
 * 
 * 
 * 
 * @author Beat Sturzenegger
 * @date 2022-02-13
 * @copyright Copyright (c) 2022
 * 
 */