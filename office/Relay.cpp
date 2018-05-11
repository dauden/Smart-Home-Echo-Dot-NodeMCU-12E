#include "Relay.h"

Relay::Relay() {
  Serial.println("I'm ready!!!");
  Serial.println("default constructor called");
}

Relay::Relay(String alexaInvokeName, unsigned int port, unsigned int pinControl, unsigned int pinState) {
  uint32_t chipId = ESP.getChipId();
  char uuid[64];
  sprintf_P(uuid, PSTR("38323636-4558-4dda-9189-cda0e6%02x%02x%02x"),
            (uint16_t) ((chipId >> 16) & 0xff),
            (uint16_t) ((chipId >>  8) & 0xff),
            (uint16_t)   chipId        & 0xff);

  serial = String(uuid);
  persistentUuid = "Socket-1_0-" + serial + "-" + String(port);

  deviceName = alexaInvokeName;
  localPort = port;
  relayPinControl = pinControl;
  relayPinState = pinState;

  startWebServer();
}


//<<destructor>>
Relay::~Relay() {
  //to do : not do anything
}

void Relay::serverLoop() {
  if (server != NULL) {
    server->handleClient();
    delay(1);
  }
}

void Relay::startWebServer() {
  server = new ESP8266WebServer(localPort);

  server->on("/", [&]() {
    handleRoot();
  });


  server->on("/setup.xml", HTTP_GET, [&]() {
    handleSetupXml();
  });

  server->on("/upnp/control/basicevent1", HTTP_POST, [&]() {
    handleUpnpControl();
  });

  server->on("/eventservice.xml", HTTP_GET, [&]() {
    handleEventservice();
  });

  server->on("/switch", HTTP_GET, [&]() {
    Serial.println("should handler by switch function!!!");
    handleRelay();
  });

  //server->onNotFound(handleNotFound);
  server->begin();
  Serial.println("WebServer started on port: ");
  Serial.println(localPort);
}

void Relay::handleEventservice() {
  Serial.println(" ########## Responding to eventservice.xml ... ########\n");

  String eventservice_xml = "<?scpd xmlns=\"urn:Belkin:service-1-0\"?>"
                            "<actionList>"
                            "<action>"
                            "<name>SetBinaryState</name>"
                            "<argumentList>"
                            "<argument>"
                            "<retval/>"
                            "<name>BinaryState</name>"
                            "<relatedStateVariable>BinaryState</relatedStateVariable>"
                            "<direction>in</direction>"
                            "</argument>"
                            "</argumentList>"
                            "<serviceStateTable>"
                            "<stateVariable sendEvents=\"yes\">"
                            "<name>BinaryState</name>"
                            "<dataType>Boolean</dataType>"
                            "<defaultValue>0</defaultValue>"
                            "</stateVariable>"
                            "<stateVariable sendEvents=\"yes\">"
                            "<name>level</name>"
                            "<dataType>string</dataType>"
                            "<defaultValue>0</defaultValue>"
                            "</stateVariable>"
                            "</serviceStateTable>"
                            "</action>"
                            "</scpd>\r\n"
                            "\r\n";

  server->send(200, "text/plain", eventservice_xml.c_str());
}

void Relay::handleUpnpControl() {
  Serial.println("########## Responding to  /upnp/control/basicevent1 ... ##########");

  String request = server->arg(0);
  Serial.print("request:");
  Serial.println(request);

  if (request.indexOf("SetBinaryState") >= 0) {
    if (request.indexOf("<BinaryState>1</BinaryState>") >= 0) {
      Serial.println("Got Turn on request");
      turnOnRelay();
    }
    if (request.indexOf("<BinaryState>0</BinaryState>") >= 0) {
      Serial.println("Got Turn off request");
      turnOffRelay();
    }
  }

  respondToRequest();
}

void Relay::handleRoot() {
  server->send(200, "text/plain", "You should tell Alexa to discover devices");
}

void Relay::handleRelay() {
  Serial.println("########## Responding to switch on/off get request ... ##########");
  String request = server->arg(0);
  Serial.println("req: >> " + request);
  if (request != "") {
    
    if (request == "true") {
      Serial.println("Got switch Turn on request");
      turnOnRelay();
    }

    if (request == "false") {
      Serial.println("Got switch Turn off request");
      turnOffRelay();
    }
  }
  respondJsonToRequest();
}

void Relay::handleSetupXml() {
  Serial.println(" ########## Responding to setup.xml ... ########\n");

  IPAddress localIP = WiFi.localIP();
  char s[16];
  sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

  String setup_xml = "<?xml version=\"1.0\"?>"
                     "<root>"
                     "<device>"
                     "<deviceType>urn:Belkin:device:controllee:1</deviceType>"
                     "<friendlyName>" + deviceName + "</friendlyName>"
                     "<manufacturer>Belkin International Inc.</manufacturer>"
                     "<modelName>Emulated Socket</modelName>"
                     "<modelNumber>3.1415</modelNumber>"
                     "<UDN>uuid:" + persistentUuid + "</UDN>"
                     "<serialNumber>221517K0101769</serialNumber>"
                     "<binaryState>0</binaryState>"
                     "<serviceList>"
                     "<service>"
                     "<serviceType>urn:Belkin:service:basicevent:1</serviceType>"
                     "<serviceId>urn:Belkin:serviceId:basicevent1</serviceId>"
                     "<controlURL>/upnp/control/basicevent1</controlURL>"
                     "<eventSubURL>/upnp/event/basicevent1</eventSubURL>"
                     "<SCPDURL>/eventservice.xml</SCPDURL>"
                     "</service>"
                     "</serviceList>"
                     "</device>"
                     "</root>\r\n"
                     "\r\n";

  server->send(200, "text/xml", setup_xml.c_str());

  Serial.print("Sending :");
  Serial.println(setup_xml);
}

String Relay::getAlexaInvokeName() {
  return deviceName;
}

void Relay::respondToSearch(IPAddress& senderIP, unsigned int senderPort) {
  Serial.println("");
  Serial.print("Sending response to ");
  Serial.println(senderIP);
  Serial.print("Port : ");
  Serial.println(senderPort);

  IPAddress localIP = WiFi.localIP();
  char s[16];
  sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

  String response =
    "HTTP/1.1 200 OK\r\n"
    "CACHE-CONTROL: max-age=86400\r\n"
    "DATE: Sat, 26 Nov 2016 04:56:29 GMT\r\n"
    "EXT:\r\n"
    "LOCATION: http://" + String(s) + ":" + String(localPort) + "/setup.xml\r\n"
    "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
    "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n"
    "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
    "ST: urn:Belkin:device:**\r\n"
    "USN: uuid:" + persistentUuid + "::urn:Belkin:device:**\r\n"
    "X-User-Agent: redsonic\r\n\r\n";

  UDP.beginPacket(senderIP, senderPort);
  UDP.write(response.c_str());
  UDP.endPacket();

  Serial.println("Response sent !");
}

void Relay::respondJsonToRequest() {
  getRelayState();
  String body = "{ \"On\":";
  body += (relayState ? "true" : "false");
  body +=  "}";

  server->send(200, "text/xml", body.c_str());
}

void Relay::respondToRequest() {
  getRelayState();
  String body =
    "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><s:Body>\r\n"
    "<u:GetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">\r\n"
    "<BinaryState>";

  body += (relayState ? "1" : "0");

  body += "</BinaryState>\r\n"
          "</u:GetBinaryStateResponse>\r\n"
          "</s:Body> </s:Envelope>\r\n";

  server->send(200, "text/xml", body.c_str());
}


int Relay::getMaxSensorValue()
{
  int sensorValue; //value read from the sensor
  int sensorMax = 0;
  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) //sample for 1000ms
  {
    sensorValue = analogRead(relayPinState);
    if (sensorValue > sensorMax)
    {
      /*record the maximum sensor value*/
      sensorMax = sensorValue;
    }
  }
  return sensorMax;
}

void Relay::getRelayState() {
  relayState = digitalRead(relayPinControl);
//  sensorMax = getMaxSensorValue();
//  Serial.print("sensorMax = ");
//  Serial.println(sensorMax);
//  //the VCC on the Grove interface of the sensor is 5v
//  amplitudeCurrent = (float)sensorMax / 1024 * 5 / 200 * 1000000;
//  effectiveValue = amplitudeCurrent / 1.414;
//  Serial.println("The effective value of the current is(in mA)");
//  Serial.println(effectiveValue, 1);
//
//  if (effectiveValue > deviceOnCurrentThreshold) {
//    Serial.println("Relay is on");
//    relayState = 1 ;
//  } else {
//    Serial.println("Relay is off");
//    relayState = 0 ;
//  }
}
void Relay::turnOnRelay() {
  Serial.print("Request turn relay on ...");
  getRelayState();
  if (relayState == 0) {
    Serial.print("Turn relay is on...");
    digitalWrite(relayPinControl, !digitalRead(relayPinControl));   // set relay on
  } else {
    Serial.print("Relay was on...");
  }
}
void Relay::turnOffRelay() {
  Serial.print("Request turn relay off ...");
  getRelayState();
  if (relayState == 1) {
  Serial.print("Turn relay is off...");
    digitalWrite(relayPinControl, !digitalRead(relayPinControl));   // set relay on
  } else {
    Serial.print("Relay was off...");
  }
}
