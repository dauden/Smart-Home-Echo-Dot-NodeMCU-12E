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

// Set Relay Pins
int relayOneControl = 12; //D6=GPI012
int relayOneState = 3; //D9=GPI03
int relayTwoControl = 13; //D7=GPI013
int relayTwoState = 4; //D2=GPI04
int relayThreeControl = 14; //D5=GPI014
int relayThreeState = 5; //D1=GPI05

void setup()
{
  Serial.begin(115200);
   
  // Initialise wifi connection
  wifiConnected = wifiManagement.connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your Devicees here. Max 14
    // Format: Alexa invocation name, local port no, control pin, get state pin
    deviceOne = new Relay("Study Light", 81, relayOneControl, relayOneState);
    deviceTwo = new Relay("Bedroom Outlet", 82, relayTwoControl, relayTwoState);
    deviceThree = new Relay("Bedroom Fan", 83, relayThreeControl, relayThreeState);
    
    Serial.println("Adding Devicees upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*deviceOne);
    upnpBroadcastResponder.addDevice(*deviceTwo);
    upnpBroadcastResponder.addDevice(*deviceThree);
    
  }
}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      deviceOne->serverLoop();
      deviceTwo->serverLoop();
      deviceThree->serverLoop();
	 }
   else {
      setup();
      delay(5000);
   }
}
