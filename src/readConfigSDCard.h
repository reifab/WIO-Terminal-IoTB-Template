#ifndef READCONFIG_H
#define READCONFIG_H

void readConfigFromSDCard();
const char *getWioTerminalID();
const char *getWifiSSID();
const char *getWifiPW();
const char *getMQTTBroker();
const char *getMQTTUser();
const char *getMQTTPW();

#endif // READCONFIG_H
