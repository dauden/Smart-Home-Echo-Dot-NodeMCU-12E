#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUDP.h>

class Relay {
private:
        ESP8266WebServer *server = NULL;
        WiFiUDP UDP;
        String serial;
        String persistentUuid;
        String deviceName;
        unsigned int localPort;
        unsigned int relayPinControl;
        unsigned int relayPinState;
        float amplitudeCurrent; //amplitude current
        float effectiveValue; //effective current
        int sensorMax;
        int deviceOnCurrentThreshold = 16;
        int relayState;

        void startWebServer();
        void handleEventservice();
        void handleUpnpControl();
        void handleRoot();
        void handleSetupXml();
        void handleRelay();

        int getMaxSensorValue();
        void getRelayState();
        void turnOnRelay();
        void turnOffRelay();
        
public:
        Relay();
        Relay(String alexaInvokeName, unsigned int port, unsigned int pinControl, unsigned int pinState);
        ~Relay();
        String getAlexaInvokeName();
        void serverLoop();
        void respondToSearch(IPAddress& senderIP, unsigned int senderPort);
        void respondToRequest();
        void respondJsonToRequest();
};

#endif
