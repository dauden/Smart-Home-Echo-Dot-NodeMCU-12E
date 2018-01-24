/*
  WifiManagement.h - Library for wifi connection code.
  Created by Anh Nguyen <anh.nguyen@codingland.com>.
*/
#ifndef WIFIMANAGEMENT_H
#define WIFIMANAGEMENT_H

#include "Arduino.h"
#include <WiFiUdp.h>

class WifiManagement
{
  public:
    WifiManagement();
    void connectWifi();
};

#endif