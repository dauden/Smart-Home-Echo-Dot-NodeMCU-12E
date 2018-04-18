#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <functional>
#include "WifiManagement.h"
#include "Relay.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

//on/off callbacks 
void turnOnRelay();
void turnOffRelay();
void getRelayState();

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;
WifiManagement wifiManagement;

Relay *deviceOne = NULL;

// Set Relay Pins
int relayOne = 12; //D6=GPI012
int relayTwo = 13; //D7=GPI013
int relayThree = 14; //D5=GPI014

void setup()
{
  Serial.begin(115200);
   
  // Initialise wifi connection
  wifiConnected = wifiManagement.connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your Devicees here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    deviceOne = new Relay("Kids light", 81, turnOnRelay, turnOffRelay, getRelayState);
    
    Serial.println("Adding Devicees upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*deviceOne);

    //Set relay pins to outputs
    pinMode(relayOne,OUTPUT); 
    
    //Set each relay pin to HIGH
    digitalWrite(relayOne, HIGH);   // sets relayOne on
    delay(500);
  }
}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();
      
      deviceOne->serverLoop();
	 }
   else {
      setup();
      delay(5000);
   }
}

void turnOnRelay(boolean json) {
    Serial.print("Device 1 turn on ...");
    digitalWrite(relayOne, !digitalRead(relayOne));   // sets relayOne on
    if(json) {
      deviceOne->respondJsonToRequest(!digitalRead(relayOne)); 
    } 
    else {
     deviceOne->respondToRequest(!digitalRead(relayOne)); 
    }
}

void turnOffRelay(boolean json) {
    Serial.print("Device 1 turn off ...");
    digitalWrite(relayOne, !digitalRead(relayOne));   // sets relayOne off
    if(json) {
      deviceOne->respondJsonToRequest(!digitalRead(relayOne)); 
    } 
    else {
     deviceOne->respondToRequest(!digitalRead(relayOne)); 
    }
}

void getRelayState(boolean json) {
    Serial.print("Device 1 get state ...");
    if(json) {
      deviceOne->respondJsonToRequest(digitalRead(relayOne)); 
    } 
    else {
     deviceOne->respondToRequest(digitalRead(relayOne)); 
    }
}
