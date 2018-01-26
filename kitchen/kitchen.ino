#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <functional>
#include "WifiManagement.h"
#include "Device.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

//on/off callbacks 
void deviceOneOn();
void deviceOneOff();
void deviceTwoOn();
void deviceTwoOff();

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;
WifiManagement wifiManagement;

Device *deviceOne = NULL;
Device *deviceTwo = NULL;

// Set Relay Pins
int relayOne = 12; //D6=GPI012
int relayTwo = 13; //D7=GPI013

void setup()
{
  Serial.begin(115200);
   
  // Initialise wifi connection
  wifiConnected = wifiManagement.connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your Devicees here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    deviceOne = new Device("Kitchen Fan", 81, deviceOneOn, deviceOneOff);
    deviceTwo = new Device("Kitchen Light", 82, deviceTwoOn, deviceTwoOff);
    
    Serial.println("Adding Devicees upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*deviceOne);
    upnpBroadcastResponder.addDevice(*deviceTwo);
    
    //Set relay pins to outputs
    pinMode(12,OUTPUT); 
    pinMode(13,OUTPUT);

    //Set each relay pin to HIGH
    digitalWrite(relayOne, HIGH);   // sets relayOne on
    delay(500);
    digitalWrite(relayTwo, HIGH);   // sets relayOne on
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
}

void deviceOneOn() {
    Serial.print("Device 1 turn on ...");
    digitalWrite(relayOne, !digitalRead(relayOne));   // sets relayOne on
}

void deviceOneOff() {
    Serial.print("Device 1 turn off ...");
    digitalWrite(relayOne, !digitalRead(relayOne));   // sets relayOne off
}

void deviceTwoOn() {
    Serial.print("Device 2 turn on ...");
    digitalWrite(relayTwo, !digitalRead(relayTwo));   // sets relayTwo on
}

void deviceTwoOff() {
  Serial.print("Device 2 turn off ...");
  digitalWrite(relayTwo, !digitalRead(relayTwo));   // sets relayTwo on
}
