#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "WifiManagement.h"
#include "Relay.h"
#include "UpnpBroadcastResponder.h"

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;
WifiManagement wifiManagement;

Relay *deviceOne = NULL;
Relay *deviceTwo = NULL;

// Set Relay Pins
int relayOneControl = 12; //D6=GPI012
int relayOneState = 3; //D9=GPI03
int relayTwoControl = 13; //D7=GPI013
int relayTwoState = 4; //D2=GPI04

void setup()
{
  Serial.begin(115200);
   
  // Initialise wifi connection
  wifiConnected = wifiManagement.connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your Devicees here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    deviceOne = new Relay("Bedroom Light", 81, relayOneControl, relayOneState);
    deviceTwo = new Relay("Stair Light", 82, relayTwoControl, relayTwoState);
    
    Serial.println("Adding Devicees upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*deviceOne);
    upnpBroadcastResponder.addDevice(*deviceTwo);
    deviceOne->initing();
    deviceTwo->initing();

  }
}
 
void loop()
{
   if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      deviceOne->serverLoop();
      deviceTwo->serverLoop();
   }
   else {
      setup();
      delay(5000);
   }
}
