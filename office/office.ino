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
void deviceOneState();
void deviceTwoOn();
void deviceTwoOff();
void deviceTwoState();

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
    deviceOne = new Relay("Office Light", 81, deviceOneOn, deviceOneOff, deviceOneState);
    deviceTwo = new Relay("Office Fan", 82, deviceTwoOn, deviceTwoOff, deviceTwoState);
    
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

void deviceOneOn() {
    Serial.print("Request turn device 1 on ...");
    int state = deviceOne->getRelayState(lineOneState));
    if(state == 0){
      Serial.print("Turn device one is on...");
      digitalWrite(relayOne, !digitalRead(relayOne));   // sets relayOne on 
    } else {
      Serial.print("Device one is on...");
    }
}

void deviceOneOff() {
    Serial.print("Request turn device 1 off ...");
    int state = deviceOne->getRelayState(lineOneState));
    if(state == 1){
      Serial.print("Turn device one is off...");
      digitalWrite(relayOne, !digitalRead(relayOne));   // sets relayOne on 
    } else {
      Serial.print("Device one is off...");
    }
}

void deviceTwoOn() {
    Serial.print("Request turn device 2 on ...");
    int state = deviceTwo->getRelayState(lineTwoState));
    if(state == 0){
      Serial.print("Turn device one is on...");
      digitalWrite(relayTwo, !digitalRead(relayTwo));   // sets relayTwo on 
    } else {
      Serial.print("Device one is on...");
    }
}

void deviceTwoOff() {
   Serial.print("Request turn device 2 off ...");
    int state = deviceTwo->getRelayState(lineTwoState));
    if(state == 1){
      Serial.print("Turn device one is off...");
      digitalWrite(relayTwo, !digitalRead(relayTwo));   // sets relayTwo on 
    } else {
      Serial.print("Device one is off...");
    }
}

void deviceOneState() {
    Serial.print("Device 3 turn on ...");
}

void deviceTwoState() {
    Serial.print("Device 3 turn off ...");
}
