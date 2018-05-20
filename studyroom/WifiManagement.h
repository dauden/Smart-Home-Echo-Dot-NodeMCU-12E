/*
  WifiManagement.h - Library for wifi connection code.
  Created by Anh Nguyen <anh.nguyen@codingland.com>.
*/
#ifndef WIFIMANAGEMENT_H
#define WIFIMANAGEMENT_H

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <functional>

class WifiManagement
{
  public:
    WifiManagement();
    ~WifiManagement();
    boolean connectWifi();
};

#endif
