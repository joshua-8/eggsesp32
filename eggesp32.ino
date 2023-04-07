/**
   This program hosts an API that returns the number of eggs that have passed a beam break sensor.
   at ipaddress:80/eggs
   v1.0 by Joshua Phelps 2023-04-07
*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "Brown-Guest";

WebServer server(80);

byte beamBreakPin = 0;
byte LED_BUILTIN = 2;

int numEggs = 0;
boolean lastBeamBreakState = HIGH;

void handleRoot() {
  digitalWrite(LED_BUILTIN, HIGH);
  char msg[50];
  sprintf(msg, "{\"numEggs\":%d}", numEggs);
  server.send(200, "application/json", msg);
  digitalWrite(LED_BUILTIN, LOW);
}


void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(beamBreakPin,INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());

  if (MDNS.begin("eggsp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/eggs", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  boolean beamBreakState = digitalRead(beamBreakPin);
  if (beamBreakState == LOW && beamBreakState != lastBeamBreakState) {
    numEggs++;
  }
  lastBeamBreakState = beamBreakState;
  server.handleClient();
  Serial.println(numEggs);
  delay(5);//allow the cpu to switch to other tasks
}
