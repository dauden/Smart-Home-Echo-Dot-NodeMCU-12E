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
Relay *deviceThree = NULL;
Relay *deviceFour = NULL;

// Set Relay Pins
int relayOneControl = 12; //D6=GPI012
int relayOneState = 3; //D9=GPI03
int relayTwoControl = 13; //D7=GPI013
int relayTwoState = 4; //D2=GPI04
int relayThreeControl = 14; //D5=GPI014
int relayThreeState = 5; //D1=GPI05
int relayFourControl = 15; //D8=GPI015
int relayFourState = 2; //D4=GPI02

void setup()
{
  Serial.begin(115200);
   
  // Initialise wifi connection
  wifiConnected = wifiManagement.connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your Devicees here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    deviceOne = new Relay("Living Left", 81, relayOneControl, relayOneState);
    deviceTwo = new Relay("Living Right", 82, relayTwoControl, relayTwoState);
    deviceThree = new Relay("Living Center", 83, relayThreeControl, relayThreeState);
    deviceFour = new Relay("Living Outlet", 84, relayFourControl, relayFourState);

    Serial.println("Adding Devicees upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*deviceOne);
    upnpBroadcastResponder.addDevice(*deviceTwo);
    upnpBroadcastResponder.addDevice(*deviceThree);
    upnpBroadcastResponder.addDevice(*deviceFour);

  }
}
 
void loop()
{
   if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      deviceOne->serverLoop();
      deviceTwo->serverLoop();
      deviceThree->serverLoop();
      deviceFour->serverLoop();
   }
   else {
      setup();
      delay(5000);
   }
}
