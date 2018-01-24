#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <functional>
#include "../libs/WifiManagement.h"
#include "../libs/Device.h"
#include "../libs/UpnpBroadcastResponder.h"
#include "../libs/CallbackFunction.h"

//on/off callbacks 
void deviceOneOn();
void deviceOneOff();
void deviceTwoOn();
void deviceTwoOff();
void deviceThreeOn();
void deviceThreeOff();
void deviceFourOn();
void deviceFourOff();

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;
WifiManagement wifiManagement;

Device *deviceOne = NULL;
Device *deviceTwo = NULL;
Device *deviceThree = NULL;
Device *deviceFour = NULL;

// Set Relay Pins
int relayOne = 12;
int relayTwo = 13;
int relayThree = 14;
int relayFour = 16;

void setup()
{
  Serial.begin(115200);
   
  // Initialise wifi connection
  wifiConnected = wifiManagement.connectWifi();
  
  if(wifiConnected){
    upnpBroadcastResponder.beginUdpMulticast();
    
    // Define your Devicees here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    deviceOne = new Device("Living Left", 80, deviceOneOn, deviceOneOff);
    deviceTwo = new Device("Living Light", 81, deviceTwoOn, deviceTwoOff);
    deviceThree = new Device("Living Center", 82, deviceThreeOn, deviceThreeOff);
    deviceFour = new Device("Living Outlet", 83, deviceFourOn, deviceFourOff);

    Serial.println("Adding Devicees upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*deviceOne);
    upnpBroadcastResponder.addDevice(*deviceTwo);
    upnpBroadcastResponder.addDevice(*deviceThree);
    upnpBroadcastResponder.addDevice(*deviceFour);

    //Set relay pins to outputs
    pinMode(12,OUTPUT); 
    pinMode(13,OUTPUT);
    pinMode(14,OUTPUT);
    pinMode(16,OUTPUT);

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

void deviceThreeOn() {
    Serial.print("Device 3 turn on ...");
    digitalWrite(relayThree, !digitalRead(relayThree));   // sets relayThree on
}

void deviceThreeOff() {
    Serial.print("Device 3 turn off ...");
    digitalWrite(relayThree, !digitalRead(relayThree));   // sets relayThree off
}

void deviceFourOn() {
    Serial.print("Device 4 turn on ...");
    digitalWrite(relayFour, !digitalRead(relayFour));   // sets relayFour on
}

void deviceFourOff() {
  Serial.print("Device 4 turn off ...");
  digitalWrite(relayFour, !digitalRead(relayFour));   // sets relayFour on
}