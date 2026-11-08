#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include <SiderealPlanets.h>
#include <MotorController.h>

#include "index.h"

#define RGB_BUILTIN 8

// --- BARRAMENTO UART PARTILHADO ---
#define RX_PIN 10
#define TX_PIN 1

// --- MOTOR AZIMUTE (Motor 1 / Driver 0b00) ---
#define AZM_STEP_PIN 3
#define AZM_DIR_PIN  0
#define AZM_DIAG_PIN 4
#define AZM_ADDR     0b00

// --- MOTOR ALTITUDE (Motor 2 / Driver 0b01) ---
#define ALT_STEP_PIN 7
#define ALT_DIR_PIN  6
#define ALT_DIAG_PIN 5
#define ALT_ADDR     0b01

WebServer server(80);
SiderealPlanets astro;

// Cliente global ativo para Server-Sent Events (SSE)
WiFiClient sseClient;

// Instanciação dos dois controladores de eixo
MotorController azimuthMotor(MotorAxis::AZIMUTH, AZM_STEP_PIN, AZM_DIR_PIN, AZM_DIAG_PIN, AZM_ADDR);
MotorController altitudeMotor(MotorAxis::ALTITUDE, ALT_STEP_PIN, ALT_DIR_PIN, ALT_DIAG_PIN, ALT_ADDR);

// Função auxiliar para enviar mensagens SSE para a interface Web
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

// Endpoint SSE: Mantém a ligação viva para streaming de dados
void handleEvents() {
  if (sseClient) {
    sseClient.stop(); // Fecha ligação antiga se surgir uma nova
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
    sendStatus("Coordenadas recebidas. Calculando parâmetros de alinhamento...");

    double ra = server.arg("ra").toDouble();
    double dec = server.arg("dec").toDouble();
    
    astro.setLatLong(-25.4284, -49.2733); // Curitiba
    astro.setGMTdate(2026, 9, 19);        // Data de teste
    astro.setGMTtime(23, 11, 0.0);        
    
    astro.setRAdec(ra, dec);
    astro.doRAdec2AltAz();
    
    double azimuth = astro.getAzimuth();
    double altitude = astro.getAltitude();

    Serial.printf("Alvo Recebido -> RA: %.2f, Dec: %.2f\n", ra, dec);
    Serial.printf("Calculado       -> Azimute (Pan): %.2f°, Altitude (Tilt): %.2f°\n\n", azimuth, altitude);

    neopixelWrite(RGB_BUILTIN, 0, 64, 0); // Verde durante o cálculo

    String jsonResponse = "{\"pan\":" + String(azimuth) + ", \"tilt\":" + String(altitude) + "}";
    server.send(200, "application/json", jsonResponse);

    // Movimentação Motor 1: Azimute
    sendStatus("Posição calculada. Rotacionando primeiro eixo (Azimute)...");
    azimuthMotor.moveToAngle(azimuth);

    neopixelWrite(RGB_BUILTIN, 0, 0, 64); // Azul entre eixos

    // Movimentação Motor 2: Altitude
    sendStatus("Eixo Azimute alinhado. Rotacionando segundo eixo (Altitude)...");
    altitudeMotor.moveToAngle(altitude);

    sendStatus("Sequência de alinhamento concluída com sucesso!");
    neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Desliga LED
  } else {
    server.send(400, "text/plain", "Missing coordinates");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RGB_BUILTIN, OUTPUT);
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);

  Serial.println("\n--- Starting Astro Tracker (Almagesto) ---");

  // 1. Inicializa o barramento UART físico nos GPIOs 10 (RX) e 1 (TX)
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  // 2. Inicializa os drivers TMC2209 com 600mA de corrente RMS
  azimuthMotor.begin(&Serial1, 600);
  altitudeMotor.begin(&Serial1, 600);

  // 3. Executa a calibração Sensorless Homing (StallGuard4) nos dois eixos
  Serial.println("Executando calibração (Homing) do eixo Azimute...");
  azimuthMotor.calibrate(LOW);

  Serial.println("Executando calibração (Homing) do eixo Altitude...");
  altitudeMotor.calibrate(LOW);

  Serial.println("Ambos os motores calibrados no ponto ZERO!");

  // 4. Configura o Access Point Wi-Fi e o WebServer
  WiFi.softAP("Almagesto-Control", "12345678");
  Serial.print("Access Point pronto. IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/events", handleEvents);
  server.on("/target", handleTarget);
  server.begin();

  // LED Flash de confirmação do sistema pronto
  neopixelWrite(RGB_BUILTIN, 0, 0, 64);
  delay(200);
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);
}

void loop() {
  server.handleClient();
}