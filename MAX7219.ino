// Simple text scroller project with Web server configuration
// Author: Csongor Varga
// Github: https://github.com/nygma2004/max7219scroller

// Using the MAX7219 library of:
// MAX7219_Dot_Matrix library - sideways scrolling
// Author: Nick Gammon
// Date: 2 October 2015

#include <ESP8266WiFi.h>            // ESP8266 Wifi library
#include <ESP8266WebServer.h>       // Web server functions
#include <ESP8266mDNS.h>            // Web server functions
#include <SPI.h>
#include <bitBangedSPI.h>
#include <MAX7219_Dot_Matrix.h>

const byte chips = 4;               // Number of 8x8 modules linked together
const int msglen = 500;             // Maximum length of the message

// Set your wifi credetials here, for open wifi leave password empty
const char* ssid = "xxx";
const char* password = "yyy";

// You can comment this section out for using DHCP
IPAddress ip(192, 168, 1, 85); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 254); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network

// n chips (display modules), hardware SPI with load on GPIO2
MAX7219_Dot_Matrix display (chips, 2);  // Chips / LOAD 
WiFiClient espClient;
MDNSResponder mdns;
ESP8266WebServer server(80);

char message [msglen] = "Visit website to customize";
unsigned long lastMoved = 0;
unsigned long MOVE_INTERVAL = 40;  // Default delay in miliseconds
byte intensity = 4;                // Default intensity 0-15

int  messageOffset;
String webPage = "";
String webStat = "";
String webFooter = "";
String temp = "";
 
void setup() {
  Serial.begin(115200);
  display.begin();
  display.setIntensity(intensity);
  display.sendSmooth ("", 0);

  WiFi.config(ip, gateway, subnet);   // Comment this line for using DHCP
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  temp = "Visit http://";
  temp += WiFi.localIP().toString();
  temp +=" to customize";
  temp.toCharArray(message, msglen);
  Serial.print("Signal [RSSI]: ");
  Serial.println(WiFi.RSSI());

  if (mdns.begin("max7219_display", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }  
  server.on("/", handleMainPage);
  server.on("/set", handleSetCommand);        // Handling parameter set
  server.begin();
  Serial.println("HTTP server started");
  messageOffset = - chips * 8;
}  // end of setup



void handleMainPage() {
  webPage = "<html><body><h1>MAX7219 Display and Text Scroller</h1><p>This sketch scrolls a custom message on a MAX7219 driven 8x8 pixel display units chained one after the other.</p>";
  webPage += "<p><b>Change text</b><br/>Current text: ";
  webPage += message;
  webPage += "<br/><form action=\"/set\"><input type=\"text\" name=\"text\" value=\"";
  webPage += message;
  webPage += "\"><input type=\"submit\" value=\"Update\"></form></p>";
  
  webPage += "<p><b>Change scroll speed</b><br/>Current delay: ";
  webPage += MOVE_INTERVAL;
  webPage += "<br/>Change to: <a href=\"set?delay=10\">10</a> | <a href=\"set?delay=20\">20</a> | <a href=\"set?delay=30\">30</a> | <a href=\"set?delay=40\">40</a> | <a href=\"set?delay=50\">50</a> | <a href=\"set?delay=60\">60</a> | <a href=\"set?delay=70\">70</a> | <a href=\"set?delay=80\">80</a> | <a href=\"set?delay=90\">90</a> | <a href=\"set?delay=100\">100</a>";
  webPage += "</p>";

  webPage += "<p><b>Change brightness</b><br/>Current brightness: ";
  webPage += intensity;
  webPage += "<br/>Change to: <a href=\"set?intensity=1\">1</a> | <a href=\"set?intensity=2\">2</a> | <a href=\"set?intensity=3\">3</a> | <a href=\"set?intensity=4\">4</a> | <a href=\"set?intensity=5\">5</a> | <a href=\"set?intensity=6\">6</a> | <a href=\"set?intensity=7\">7</a> | <a href=\"set?intensity=8\">8</a> | <a href=\"set?intensity=9\">9</a> | <a href=\"set?intensity=10\">10</a> | <a href=\"set?intensity=11\">11</a> | <a href=\"set?intensity=12\">12</a> | <a href=\"set?intensity=13\">13</a> | <a href=\"set?intensity=14\">14</a> | <a href=\"set?intensity=15\">15</a>";
  webPage += "</p>";

  webStat = "<p style=\"font-size: 90%; color: #FF8000;\">RSSI: ";
  webStat += WiFi.RSSI();
  webStat += "<br/>";
  webStat += "Uptime [min]: ";
  webStat += millis() / (1000*60);
  webStat += "</p>";  
      
  webFooter = "<p style=\"font-size: 80%; color: #08088A;\">MAX7219 Text Scroller v1.0 | <a href=\"mailto:csongor.varga@gmail.com\">email me</a> | <a href=\"https://github.com/nygma2004/max7219scroller\">GitHub</a></p></body></html>";  
  server.send(200, "text/html", webPage+webStat+webFooter); 
  Serial.println("Web page request");
}

void handleSetCommand() { 
  String response = "";
  if (server.args() == 0) {
    response = "No parameter";
  } else {
    if (server.argName(0) == "delay") {
      MOVE_INTERVAL = server.arg("delay").toInt();
      response = "Delay updated to ";
      response += MOVE_INTERVAL;
    }
    if (server.argName(0) == "intensity") {
      intensity = (byte)server.arg("intensity").toInt();
      response = "Intensity updated to ";
      display.setIntensity(intensity);
      response += intensity;
    }
    if (server.argName(0) == "text") {
      server.arg("text").toCharArray(message, msglen);
      messageOffset = - chips * 8;
      response = "Message updated";
    }
    if (response == "" ) {
      response = "invalid parameter";
    }
  }
  Serial.print("Change request: ");
  Serial.println(response);
  response = "<html><head><meta http-equiv=\"refresh\" content=\"2; url=/\"></head><body>" + response + "</body></html>";
  server.send(200, "text/html", response);          //Returns the HTTP response
}


void updateDisplay() {
  display.sendSmooth (message, messageOffset);
  
  // next time show one pixel onwards
  if (messageOffset++ >= (int) (strlen (message) * 8))
    messageOffset = - chips * 8;
}  // end of updateDisplay

void loop() { 


    
  // update display if time is up
  if (millis () - lastMoved >= MOVE_INTERVAL) {
    updateDisplay ();
    lastMoved = millis ();
  }

  // Handle HTTP server requests
  server.handleClient();
     
}  // end of loop

