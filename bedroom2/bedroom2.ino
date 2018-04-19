#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <functional>
#include "WifiManagement.h"
#include "Relay.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

//on/off callbacks 
void deviceOneOn();
void deviceOneOff();
void deviceTwoOn();
void deviceTwoOff();
void deviceThreeOn();
void deviceThreeOff();

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;
WifiManagement wifiManagement;

Relay *deviceOne = NULL;
Relay *deviceTwo = NULL;
Relay *deviceThree = NULL;

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
    deviceOne = new Relay("Study Light", 81, deviceOneOn, deviceOneOff);
    deviceTwo = new Relay("Bedroom Fan", 82, deviceTwoOn, deviceTwoOff);
    deviceThree = new Relay("Bedroom Outlet", 83, deviceThreeOn, deviceThreeOff);
    
    Serial.println("Adding Devicees upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*deviceOne);
    upnpBroadcastResponder.addDevice(*deviceTwo);
    upnpBroadcastResponder.addDevice(*deviceThree);
    
    //Set relay pins to outputs
    pinMode(relayOne,OUTPUT); 
    pinMode(relayTwo,OUTPUT);
    pinMode(relayThree,OUTPUT);
    
    //Set each relay pin to HIGH
    digitalWrite(relayOne, HIGH);   // sets relayOne on
    deviceOne->setRelayState(digitalRead(relayOne));
    delay(500);
    digitalWrite(relayTwo, HIGH);   // sets relayOne on
    deviceTwo->setRelayState(digitalRead(relayTwo));
    delay(500);
    digitalWrite(relayThree, HIGH);   // sets relayOne on
    deviceThree->setRelayState(digitalRead(relayThree));
    delay(500);
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

void deviceOneOn() {
    Serial.print("Device 1 turn on ...");
    digitalWrite(relayOne, !digitalRead(relayOne));   // sets relayOne on
    deviceOne->setRelayState(digitalRead(relayOne));
}

void deviceOneOff() {
    Serial.print("Device 1 turn off ...");
    digitalWrite(relayOne, !digitalRead(relayOne));   // sets relayOne off
    deviceOne->setRelayState(digitalRead(relayOne));
}

void deviceTwoOn() {
    Serial.print("Device 2 turn on ...");
    digitalWrite(relayTwo, !digitalRead(relayTwo));   // sets relayTwo on
    deviceTwo->setRelayState(digitalRead(relayTwo));
}

void deviceTwoOff() {
  Serial.print("Device 2 turn off ...");
  digitalWrite(relayTwo, !digitalRead(relayTwo));   // sets relayTwo on
  deviceTwo->setRelayState(digitalRead(relayTwo));
}

void deviceThreeOn() {
    Serial.print("Device 3 turn on ...");
    digitalWrite(relayThree, !digitalRead(relayThree));   // sets relayThree on
    deviceThree->setRelayState(digitalRead(relayThree));
}

void deviceThreeOff() {
    Serial.print("Device 3 turn off ...");
    digitalWrite(relayThree, !digitalRead(relayThree));   // sets relayThree off
    deviceThree->setRelayState(digitalRead(relayThree));
}
