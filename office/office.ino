#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <functional>
#include "WifiManagement.h"
#include "Relay.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;
WifiManagement wifiManagement;

Relay *deviceOne = NULL;
Relay *deviceTwo = NULL;

// Set Relay Pins
int relayOne = 12; //D6=GPI012
int relayOneState = 13; //D7=GPI013
int relayTwo = 14; //D5=GPI014
int relayTwoState = 2; //D4=GPI02

void setup()
{
  Serial.begin(115200);
   
  // Initialise wifi connection
  wifiConnected = wifiManagement.connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your Devicees here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    deviceOne = new Relay("Office Light", 81, relayOne, relayOneState);
    deviceTwo = new Relay("Office Fan", 82, relayTwo, relayTwoState);
    
    Serial.println("Adding Devicees upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*deviceOne);
    upnpBroadcastResponder.addDevice(*deviceTwo);
    
    //Set relay pins to outputs
    pinMode(relayOne,OUTPUT); 
    pinMode(relayOneState, INPUT);
    pinMode(relayTwo,OUTPUT);
    pinMode(relayTwoState, INPUT);
    
    //Set each relay pin to HIGH
    digitalWrite(relayOne, HIGH);   // set relayOne on
    delay(500);
    digitalWrite(relayTwo, HIGH);   // set relayTwo on
    delay(500);
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
