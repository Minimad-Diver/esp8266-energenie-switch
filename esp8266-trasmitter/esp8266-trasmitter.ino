#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <RCSwitch.h>
#include <EEPROM.h>

const char* ssid = "Marphus";
const char* password = "fafafafafa";

const int DEBUG_LED = 13;
const int TX_PIN = 2;
const int RX_PIN = 0;

// Handler defs from "handlers.ino"
void handleRoot();
void handleSetup();
void handleQuery();
void handleWriteDevices();
void handleReadDevices();
void handleCmd();
void handleCmd2();
void handleListen();
void handleNotFound();

ESP8266WebServer server(80);
RCSwitch rcSwitch = RCSwitch();

String getDeviceName() {
  return "ESPRFSwitch:" + String(ESP.getChipId());
}


String getConfigJson() {
  uint8_t len = EEPROM.read(0);
  String buf = "";

  Serial.println(String("Reading EEPROM ") + len);

  for (size_t i = 0; i < len; i++) {
    char b = (char)EEPROM.read(i + 1);
    buf += b;
  }

  return buf;
}
void setup(void) {
  EEPROM.begin(512);

  Serial.begin(115200);

    WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
    Serial.println("Connecting WIFI ");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  pinMode(DEBUG_LED, OUTPUT);

  rcSwitch.enableTransmit(TX_PIN);
  rcSwitch.enableReceive(RX_PIN);


  Serial.println("Configured");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/setup", handleSetup);
  server.on("/query", handleQuery);
  server.on("/cmd", handleCmd);
  server.on("/cmd2", handleCmd2);
  server.on("/listen", handleListen);

  server.on("/devices.json", HTTP_GET, handleReadDevices);
  server.on("/devices.json", HTTP_POST, handleWriteDevices);

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println("HTTP server started");

}

void loop(void){
  server.handleClient();
}
