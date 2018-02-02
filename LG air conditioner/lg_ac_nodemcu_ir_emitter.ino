#include <ESP8266WiFi.h>


// Wifi credentials
const char* ssid = "your_ssid";
const char* password = "your_password";


// using built-in pin led and another one to troubleshooting stuff
int ledPin = D4; // RED LED
int ledPinGreen = D6; // GREEN LED

// NodeMCU Webserver 
WiFiServer server(80);
WiFiClient client = server.available();
int value = LOW;

/// ----- AC VARIABLES ---------------------------------------------------------------
#include <IRremoteESP8266.h>
#include <IRsend.h>


IRsend irsend(14);  // An IR LED is controlled by GPIO pin 14 (D5)

// 0 : TOWER
// 1 : WALL
const unsigned int kAc_Type  = 1;

// 0 : cooling
// 1 : heating
unsigned int ac_heat = 0;

// 0 : off
// 1 : on
unsigned int ac_power_on = 0;

// 0 : off
// 1 : on --> power on
unsigned int ac_air_clean_state = 0;

// temperature : 18 ~ 30
unsigned int ac_temperature = 18;

// 0 : low
// 1 : mid
// 2 : high
// if kAc_Type = 1, 3 : change
unsigned int ac_flow = 0;

const uint8_t kAc_Flow_Tower[3] = {0, 4, 6};
const uint8_t kAc_Flow_Wall[4] = {0, 2, 4, 5};

uint32_t ac_code_to_sent;

void Ac_Send_Code(uint32_t code) {
  Serial.print("code to send : ");
  Serial.print(code, BIN);
  Serial.print(" : ");
  Serial.println(code, HEX);

  irsend.sendLG(code, 28);
}

void Ac_Activate(unsigned int temperature, unsigned int air_flow,
                 unsigned int heat) {
  ac_heat = heat;
  unsigned int ac_msbits1 = 8;
  unsigned int ac_msbits2 = 8;
  unsigned int ac_msbits3 = 0;
  unsigned int ac_msbits4;

   
  
  if (ac_heat == 1)
    ac_msbits4 = 4;  // heating
  else
    ac_msbits4 = 0;  // cooling
    
  unsigned int ac_msbits5 =  (temperature < 15) ? 0 : temperature - 15;
  unsigned int ac_msbits6;

  if (0 <= air_flow && air_flow <= 2) {
    if (kAc_Type == 0)
      ac_msbits6 = kAc_Flow_Tower[air_flow];
    else
      ac_msbits6 = kAc_Flow_Wall[air_flow];
     
  }

  // calculating using other values
  unsigned int ac_msbits7 = (ac_msbits3 + ac_msbits4 + ac_msbits5 +
                             ac_msbits6) & B00001111;
  ac_code_to_sent = ac_msbits1 << 4;
  ac_code_to_sent = (ac_code_to_sent + ac_msbits2) << 4;
  ac_code_to_sent = (ac_code_to_sent + ac_msbits3) << 4;
  ac_code_to_sent = (ac_code_to_sent + ac_msbits4) << 4;
  ac_code_to_sent = (ac_code_to_sent + ac_msbits5) << 4;
  ac_code_to_sent = (ac_code_to_sent + ac_msbits6) << 4;
  ac_code_to_sent = (ac_code_to_sent + ac_msbits7);

  Ac_Send_Code(ac_code_to_sent);

  ac_power_on = 1;
  ac_temperature = temperature;
  ac_flow = air_flow;
  
}

void Ac_Change_Air_Swing(int air_swing) {
  if (kAc_Type == 0) {
    if (air_swing == 1)
      ac_code_to_sent = 0x881316B;
    else
      ac_code_to_sent = 0x881317C;
  } else {
    if (air_swing == 1)
      ac_code_to_sent = 0x8813149;
    else
      ac_code_to_sent = 0x881315A;
  }
  Ac_Send_Code(ac_code_to_sent);
}

void Ac_Power_Down() {
  ac_code_to_sent = 0x88C0051;

  Ac_Send_Code(ac_code_to_sent);

  ac_power_on = 0;
}

void Ac_Air_Clean(int air_clean) {
  if (air_clean == '1')
    ac_code_to_sent = 0x88C000C;
  else
    ac_code_to_sent = 0x88C0084;

  Ac_Send_Code(ac_code_to_sent);

  ac_air_clean_state = air_clean;
}



/// -------END OF AC VARIABLES --------------------------------------------------------

void setup() {
  
  Serial.begin(115200);
  delay(10);
  

  pinMode(ledPin, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
 
  digitalWrite(ledPin, LOW);
  digitalWrite(ledPinGreen, LOW);
 
  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    blinkLeds();
  }
     server.begin();
  Serial.println("Server started");
  
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  
}
 
void loop() {
  
 // checking if nodemcu is connected, if not, restarts the ESP
 getWifiStatus();
 
 
 irsend.begin();
 
  
  WiFiClient client = server.available();
  if (!client) {
    return;
  }



  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  
  if (request.indexOf("/ON") != -1)  {
    // turns ON LG Air Conditioner
    ligaAC();
    value == HIGH;
  }
  if (request.indexOf("/OFF") != -1)  {
     // turns OFF LG Air Conditioner
    desligaAC();
    value == LOW;
   
    
  }
  if (request.indexOf("/REBOOT") != -1)  {
     // Restart ESP
   reboot();
    
  }


  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("<head>");
  client.println("<meta http-equiv='refresh' content='5' >");
  client.println("</head>");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
 
  if(value == HIGH) {
    client.print("<h2 style='background-color:green;color:white'>AC: ON</h2>");
  } else {
     client.print("<h2 style='background-color:red;color:white'>AC: OFF</h2>");
  }
  client.println("<br><br>");
  client.println("<a href=\"/ON\"\"><button>AC ON</button></a>");
  client.println("<a href=\"/OFF\"\"><button>AC OFF</button></a><br />");
  client.println("<br><br>");
  client.println("<a href=\"/REBOOT\"\"><button>Reboot</button></a><br />");  
  client.println("</html>");
 
  delay(1000);
  Serial.println("Client disonnected");
  Serial.println("");
 
}


 void ligaAC() {
    
    value = HIGH;
        
    Ac_Activate(18, 0, 0);
    blinkLeds();
    
 }

 void desligaAC() {
  
    value = LOW;
    Ac_Power_Down();
    blinkLeds();
    

 }

void reboot() {
  blinkLeds();
  ESP.restart();
}
  
void blinkLeds() {
 int count=0; 
 while (count < 1) {
 digitalWrite(ledPin, LOW);
 digitalWrite(ledPinGreen,HIGH);
 delay(50);
 digitalWrite(ledPin, HIGH);
 digitalWrite(ledPinGreen,LOW);

 delay(100);
  
 digitalWrite(ledPin, HIGH);

 count = count +1;
 }
  
}

void getWifiStatus() {

// Connected state = 3 (OK)
// No SSID found = 6
// Wifi disconected = 0
if (WiFi.status() == 0 || WiFi.status()== 6) {
  reboot();
  // Needs improvements
}
  
}

