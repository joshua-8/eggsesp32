/**
   Sends a post request when a beam break sensor pulls a pin low.
   Also displays the count on a 7 segment display https://learn.adafruit.com/adafruit-led-backpack
   Also displays patterns on a string of neopixel leds.
   Used for an egg scavenger hunt counter system.
   v1.0 by Joshua Phelps 2023-04
*/

#include <WiFi.h>
#include <WiFiMulti.h>

WiFiMulti WiFiMulti;
#include <HTTPClient.h>

#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment matrix = Adafruit_7segment();

#define LED_PIN    16

#define LED_COUNT 144

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

unsigned long firefliesArray[LED_COUNT] = {0};


const char* serverName = "https://eggs.fyi/api/increment-2023";

const char* ssid = "Brown-Guest";

byte beamBreakPin = 5;
byte LED_BUILTIN = 2;

int numEggs = 0;
boolean lastBeamBreakState = HIGH;

byte lightVal = 0;

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(beamBreakPin, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);
  Serial.begin(115200);
  matrix.begin(0x70);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(255);
  strip.show();            // Turn OFF all pixels ASAP

  delay(10);

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
    numEggs += 1;
    lightVal = LED_COUNT;
    digitalWrite(LED_BUILTIN, HIGH);


    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

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
  fireflies(lightVal);
  strip.show();
  if (lightVal > 0) {
    lightVal--;
  }

  matrix.writeDigitNum(0, (numEggs / 1000) % 10);
  matrix.writeDigitNum(1, (numEggs / 100) % 10);
  matrix.writeDigitNum(3, (numEggs / 10) % 10);
  matrix.writeDigitNum(4, numEggs % 10);
  matrix.writeDisplay();
}

void fireflies(byte x) {
  x = x / 2;
  int t = 250;
  byte b = 200;
  byte s = 255;
  // fireflies animation that moves outwards from the center of the strip.
  for (int i = 0; i < LED_COUNT; i++) {
    int led = i;
    if (i >= LED_COUNT / 2) {
      led = LED_COUNT - i - 1;
    }
    int prob = 0;
    if (abs(led - x) < 35) {
      prob = 10;
    }
    if (abs(led - x) < 20) {
      prob = 30;
    }
    if (x < led) {
      prob = 2;
    }
    if (x == 0) {
      prob = 0;
    }
    if (millis() - firefliesArray[i] > (unsigned long)t) {
      if (random(0, 1000) < prob) {
        firefliesArray[i] = millis();
      }
    }
  }
  for (int i = 0; i < LED_COUNT; i++) {
    int h = (firefliesArray[i] << 8) % 65535;

    if (-firefliesArray[i] + millis() <= t / 2 && -firefliesArray[i] + millis() > 0) {
      strip.setPixelColor(i, strip.ColorHSV(h, s, map(millis() - firefliesArray[i], 0, t / 2, 0, b)));
    }
    if (-firefliesArray[i] + millis() > t / 2 && -firefliesArray[i] + millis() <= t) {
      strip.setPixelColor(i, strip.ColorHSV(h, s, map(millis() - firefliesArray[i], t / 2, t, b, 0)));
    }
    if (-firefliesArray[i] + millis() > t) {
      strip.setPixelColor(i, strip.ColorHSV(0, 0, 0));
    }
  }
}
