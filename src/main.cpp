#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SiderealPlanets.h>
#include "index.h"  // Pulls in your HTML file from the include folder

#define RGB_BUILTIN 8  // YD-ESP32-C3 onboard NeoPixel

WebServer server(80);
SiderealPlanets astro;

void setRGB(uint8_t r, uint8_t g, uint8_t b) {
  neopixelWrite(RGB_BUILTIN, r, g, b);
}

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleSetColor() {
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    int r = server.arg("r").toInt();
    int g = server.arg("g").toInt();
    int b = server.arg("b").toInt();
    setRGB(r, g, b);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void testAstroMath(double ra, double dec) {
  // Set coordinates for Curitiba, PR
  astro.setLatLong(-25.4284, -49.2733);
  
  // Set current GMT date and time (Example: Sep 19, 2026, 23:11 GMT)
  // Local time in Curitiba is UTC-3, so 20:11 local = 23:11 GMT
  astro.setGMTdate(2026, 9, 19);
  astro.setGMTtime(23, 11, 0.0);
  
  astro.setRAdec(ra, dec);
  astro.doRAdec2AltAz();
  
  double panAz = astro.getAzimuth();
  double tiltAlt = astro.getAltitude();
  
  Serial.printf("Target RA: %.3f, Dec: %.3f\n", ra, dec);
  Serial.printf("Required Pan (Azimuth): %.2f degrees\n", panAz);
  Serial.printf("Required Tilt (Altitude): %.2f degrees\n", tiltAlt);
}

void setup() {
  Serial.begin(115200);
  pinMode(RGB_BUILTIN, OUTPUT);
  setRGB(0, 0, 0);

  WiFi.softAP("Almagesto-Control", "12345678");
  Serial.print("Access Point started. IP Address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/set", handleSetColor);
  server.begin();

  // Test constellation targeting (Orion: RA = 5.582h, Dec = 0.30)
  testAstroMath(5.582, 0.30);
}

void loop() {
  server.handleClient();
}