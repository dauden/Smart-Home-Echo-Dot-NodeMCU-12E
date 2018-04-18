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
        CallbackFunction turnOnRelay;
        CallbackFunction turnOffRelay;
        CallbackFunction getRelayState;

        void startWebServer();
        void handleEventservice();
        void handleUpnpControl();
        void handleRoot();
        void handleSetupXml();
        void handleSwitch();
public:
        Relay();
        Relay(String alexaInvokeName, unsigned int port, CallbackFunction tournOnRelay, CallbackFunction turnOffRelay, CallbackFunction getRelayState);
        ~Relay();
        String getAlexaInvokeName();
        void serverLoop();
        void respondToSearch(IPAddress& senderIP, unsigned int senderPort);
        void respondToRequest(unsigned int state);
        void respondJsonToRequest(unsigned int state);
};

#endif
