/**
   v1.0 by Joshua Phelps 2023-04-07
*/

#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti WiFiMulti;
#include <HTTPClient.h>

const char* serverName = "https://api.restful-api.dev/objects";

const char* ssid = "Brown-Guest";

byte beamBreakPin = 0;
byte LED_BUILTIN = 2;

int numEggs = 0;
boolean lastBeamBreakState = HIGH;



void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(beamBreakPin, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  WiFiMulti.addAP(ssid);

  Serial.println();
  Serial.println();
  Serial.print("Waiting for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);
  Serial.println(WiFi.macAddress());

  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("started");
}

void loop(void) {
  boolean beamBreakState = digitalRead(beamBreakPin);
  if (beamBreakState == LOW && beamBreakState != lastBeamBreakState) {
    numEggs++;
    digitalWrite(LED_BUILTIN, HIGH);



    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST("{\"numEggs\":" + String(numEggs) + "}");

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }

    Serial.println(numEggs);
    digitalWrite(LED_BUILTIN, LOW);

  }
  lastBeamBreakState = beamBreakState;

  delay(5);
}
