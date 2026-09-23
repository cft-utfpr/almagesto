#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SiderealPlanets.h>
#include "index.h"

#define RGB_BUILTIN 8

WebServer server(80);
SiderealPlanets astro;

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleTarget() {
  if (server.hasArg("ra") && server.hasArg("dec")) {
    double ra = server.arg("ra").toDouble();
    double dec = server.arg("dec").toDouble();
    
    astro.setLatLong(-25.4284, -49.2733); // Cwb
    astro.setGMTdate(2026, 9, 19);        // 19/09/2026
    astro.setGMTtime(23, 11, 0.0);        // 23:11
    
    astro.setRAdec(ra, dec);
    astro.doRAdec2AltAz();
    
    double panAz = astro.getAzimuth();
    double tiltAlt = astro.getAltitude();

    Serial.printf("Received Target -> RA: %.2f, Dec: %.2f\n", ra, dec);
    Serial.printf("Calculated      -> Pan: %.2f, Tilt: %.2f\n\n", panAz, tiltAlt);

    neopixelWrite(RGB_BUILTIN, 0, 64, 0);
    delay(100);
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);

    String jsonResponse = "{\"pan\":" + String(panAz) + ", \"tilt\":" + String(tiltAlt) + "}";
    server.send(200, "application/json", jsonResponse);
  } else {
    server.send(400, "text/plain", "Missing coordinates");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RGB_BUILTIN, OUTPUT);
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);

  Serial.println("\n--- Starting Astro Tracker ---");
  WiFi.softAP("Almagesto-Control", "12345678");
  Serial.print("Access Point ready. IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/target", handleTarget);
  server.begin();
}

void loop() {
  server.handleClient();
}