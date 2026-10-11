#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include <SiderealPlanets.h>
#include <MotorController.h>

#include "index.h"

#define RGB_BUILTIN 8

// Azimuth Motor Pins
#define AZM_MOTOR_IN1 3
#define AZM_MOTOR_IN2 4
#define AZM_MOTOR_IN3 5
#define AZM_MOTOR_IN4 10

// Altitude Motor Pins
// #define ALT_MOTOR_IN1 
// #define ALT_MOTOR_IN2 
// #define ALT_MOTOR_IN3 
// #define ALT_MOTOR_IN4 

WebServer server(80);
SiderealPlanets astro;

// Active global client for Server-Sent Events (SSE)
WiFiClient sseClient;

MotorController azimuthMotor(MotorAxis::AZIMUTH, AZM_MOTOR_IN1, AZM_MOTOR_IN2, AZM_MOTOR_IN3, AZM_MOTOR_IN4);
// MotorController altitudeMotor(MotorAxis::ALTITUDE, ALT_MOTOR_IN1, ALT_MOTOR_IN2, ALT_MOTOR_IN3, ALT_MOTOR_IN4);

// Helper function to send SSE messages to the frontend
void sendStatus(String message) {
  if (sseClient && sseClient.connected()) {
    sseClient.print("data: ");
    sseClient.print(message);
    sseClient.print("\n\n");
    sseClient.flush();
    Serial.println("SSE Event: " + message);
  }
}

void handleRoot() {
  server.send(200, "text/html", index_html);
}

// SSE Endpoint: Keeps the connection alive to stream data
void handleEvents() {
  if (sseClient) {
    sseClient.stop(); // Close old connection if a new one arrives
  }
  sseClient = server.client();
  sseClient.println("HTTP/1.1 200 OK");
  sseClient.println("Content-Type: text/event-stream");
  sseClient.println("Cache-Control: no-cache");
  sseClient.println("Connection: keep-alive");
  sseClient.println();
  sseClient.flush();
}

void handleTarget() {
  if (server.hasArg("ra") && server.hasArg("dec")) {
    sendStatus("Coordinates received. Calculating parameters...");

    double ra = server.arg("ra").toDouble();
    double dec = server.arg("dec").toDouble();
    
    astro.setLatLong(-25.4284, -49.2733); // Cwb
    astro.setGMTdate(2026, 9, 19);        // 19/09/2026
    astro.setGMTtime(23, 11, 0.0);        // 23:11
    
    astro.setRAdec(ra, dec);
    astro.doRAdec2AltAz();
    
    double azimuth = astro.getAzimuth();
    double altitude = astro.getAltitude();

    Serial.printf("Received Target -> RA: %.2f, Dec: %.2f\n", ra, dec);
    Serial.printf("Calculated      -> Pan: %.2f, Tilt: %.2f\n\n", azimuth, altitude);

    neopixelWrite(RGB_BUILTIN, 0, 64, 0);

    String jsonResponse = "{\"pan\":" + String(azimuth) + ", \"tilt\":" + String(altitude) + "}";
    server.send(200, "application/json", jsonResponse);

    // Motor 1: Azimuth
    sendStatus("Target computed. Rotating first axis (Azimuth)...");
    azimuthMotor.moveToAngle(azimuth);

    neopixelWrite(RGB_BUILTIN, 0, 0, 64);

    // Motor 2: Altitude
    sendStatus("First axis aligned. Starting second axis (Altitude)...");
    delay(4000); // emulate motor rotation
    // altitudeMotor.moveToAngle(altitude);

    sendStatus("Sequence complete");
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
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
  
  // altitudeMotor.begin();
  // altitudeMotor.calibrate();

  Serial.println("\n--- Starting Astro Tracker ---");
  WiFi.softAP("Almagesto-Control", "12345678");
  Serial.print("Access Point ready. IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/events", handleEvents); // Register SSE endpoint
  server.on("/target", handleTarget);
  server.begin();

  neopixelWrite(RGB_BUILTIN, 0, 0, 64);
  delay(100);
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);
}

void loop() {
  server.handleClient();
}