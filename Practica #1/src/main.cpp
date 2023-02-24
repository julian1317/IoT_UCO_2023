#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* ssid = "******";
const char* password = "*******";

ESP8266WebServer server(80);

int led1 = D1;
int led2 = D2;

void handleRoot() {
  String html = "<html><head><title>Control de LEDs</title>";
  html += "<style>button{font-size:2em; padding:0.5em 1em;}</style></head>";
  html += "<body><h1>PRACTICA #1 IOT</h1>";
  html += "<button onclick=\"led1_on()\">Encender LED 1</button>";
  html += "<button onclick=\"led1_off()\">Apagar LED 1</button><br><br>";
  html += "<button onclick=\"led2_on()\">Encender LED 2</button>";
  html += "<button onclick=\"led2_off()\">Apagar LED 2</button>";
  html += "<script>";
  html += "function led1_on(){fetch('/led1_on');}";
  html += "function led1_off(){fetch('/led1_off');}";
  html += "function led2_on(){fetch('/led2_on');}";
  html += "function led2_off(){fetch('/led2_off');}";
  html += "</script></body></html>";
  server.send(200, "text/html", html);
}

void handleLed1On() {
  digitalWrite(led1, HIGH);
  server.send(200, "text/plain", "LED 1 encendido");
}

void handleLed1Off() {
  digitalWrite(led1, LOW);
  server.send(200, "text/plain", "LED 1 apagado");
}

void handleLed2On() {
  digitalWrite(led2, HIGH);
  server.send(200, "text/plain", "LED 2 encendido");
}

void handleLed2Off() {
  digitalWrite(led2, LOW);
  server.send(200, "text/plain", "LED 2 apagado");
}

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/led1_on", handleLed1On);
  server.on("/led1_off", handleLed1Off);
  server.on("/led2_on", handleLed2On);
  server.on("/led2_off", handleLed2Off);

  server.begin();
}

void loop() {
  server.handleClient();
}
