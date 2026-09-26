#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include <SiderealPlanets.h>
#include <MotorController.h>

#include "index.h"

#define RGB_BUILTIN 8

#define AZM_MOTOR_IN1 3
#define AZM_MOTOR_IN2 4
#define AZM_MOTOR_IN3 5
#define AZM_MOTOR_IN4 10

WebServer server(80);
SiderealPlanets astro;

MotorController azimuthMotor(MotorAxis::AZIMUTH, AZM_MOTOR_IN1, AZM_MOTOR_IN2, AZM_MOTOR_IN3, AZM_MOTOR_IN4);

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
    
    double azimuth = astro.getAzimuth();
    double altitude = astro.getAltitude();

    neopixelWrite(RGB_BUILTIN, 0, 64, 0);

    azimuthMotor.moveToAngle(azimuth);

    Serial.printf("Received Target -> RA: %.2f, Dec: %.2f\n", ra, dec);
    Serial.printf("Calculated      -> Pan: %.2f, Tilt: %.2f\n\n", azimuth, altitude);

    neopixelWrite(RGB_BUILTIN, 0, 0, 0);

    String jsonResponse = "{\"pan\":" + String(azimuth) + ", \"tilt\":" + String(altitude) + "}";
    server.send(200, "application/json", jsonResponse);
  } else {
    server.send(400, "text/plain", "Missing coordinates");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RGB_BUILTIN, OUTPUT);
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);

  azimuthMotor.begin();
  azimuthMotor.calibrate();

  Serial.println("\n--- Starting Astro Tracker ---");
  WiFi.softAP("Almagesto-Control", "12345678");
  Serial.print("Access Point ready. IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/target", handleTarget);
  server.begin();

  neopixelWrite(RGB_BUILTIN, 0, 0, 64);
  delay(100);
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);
}

void loop() {
  server.handleClient();
}