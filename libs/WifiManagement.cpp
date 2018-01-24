#include "Arduino.h"
#include <WiFiUdp.h>

// Change this before you flash
const char* ssid = "Skynet";
const char* password = "8039795700";

//<<constructor>>
WifiManagement::WifiManagement(){}

//<<destructor>>
WifiManagement::~WifiManagement(){}

// connect to wifi – returns true if successful or false if not
boolean WifiManagement::connectWifi(){
  boolean state = true;
  int i = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.println("Great!! successfullly...");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Sorry but connecting unsuccessully.");
    Serial.println("Connection failed.");
  }
  
  return state;
}