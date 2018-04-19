#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUDP.h>
#include "CallbackFunction.h"

class Relay {
private:
        ESP8266WebServer *server = NULL;
        WiFiUDP UDP;
        String serial;
        String persistentUuid;
        String deviceName;
        unsigned int localPort;
        boolean relayState;
        CallbackFunction turnOnRelay;
        CallbackFunction turnOffRelay;

        void startWebServer();
        void handleEventservice();
        void handleUpnpControl();
        void handleRoot();
        void handleSetupXml();
        void handleSwitch();
public:
        Relay();
        Relay(String alexaInvokeName, unsigned int port, CallbackFunction tournOnRelay, CallbackFunction turnOffRelay);
        ~Relay();
        String getAlexaInvokeName();
        void setRelayState(boolean state);
        void serverLoop();
        void respondToSearch(IPAddress& senderIP, unsigned int senderPort);
        void respondToRequest();
        void respondJsonToRequest();
};

#endif
