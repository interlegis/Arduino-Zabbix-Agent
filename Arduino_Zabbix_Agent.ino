//******************************************************************************
//* Purpose : Zabbix Sensor Agent - Environmental Monitoring Solution *
//* Git : https://github.com/interlegis/arduino-zabbix-agent
//* Author :  Gabriel Ferreira and Marco Rougeth *
//* https://github.com/gabrielrf and
//* https://github.com/rougeth
//* Adapted from : Evgeny Levkov and Schotte Vincent *
//* Credits: *

#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>
#include <OneWire.h>
#include <string.h>

//----------------------------- Network settings -------------------------------
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xE3, 0x1B };
IPAddress ip(10, 1, 2, 235);
//IPAddress ip(10, 1, 2, 61);
//IPAddress ip(10, 1, 2, 239);
IPAddress gateway(10, 1, 2, 254);
IPAddress subnet(255, 255, 255, 0);

//------------ Pins 10, 11, 12 e 13 are used by ethernet shield! ---------------
#define MAX_CMD_LENGTH 25
#define LED_PIN 3
#define DHT11_PIN 4            // DHT11 pin
#define ONE_WIRE_PIN 5         // One wire pin with a 4.7k resistor
#define PIR_PIN 6              // PIR pin
#define SOIL_PIN A0            // Soil humidity sensor pin

EthernetServer server(10050);
EthernetClient client;
OneWire ds(ONE_WIRE_PIN);
dht DHT;

boolean connected = false;
byte i;
byte present = 0;
byte type_s;
byte data[12];
byte addr[8];
double temp = 0;          // Temperature
double umid = 0;          // Humidity
float celsius;
float oneWire17 = 0;
float oneWireB6 = 0;
float oneWireD3 = 0;
String cmd;               //FOR ZABBIX COMMAND
String serialNum;
int counter = 1;          // For testing
int chk;
int presence = 0;
int soil = 0;             // Soil humidity
int limite = 1;           // Command size. Using 1 for better performance.
unsigned long dhtLastCheck = 0;
unsigned long pirLastCheck = 0;

void readOneWire() {
  if ( !ds.search(addr)) {
    //Serial.println("No more addresses.");
    ds.reset_search();
    //delay(250);
    return;
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return;
  }
  switch (addr[0]) {
    case 0x10:
      type_s = 1;
      break;
    case 0x28:
      type_s = 0;
      break;
    case 0x22:
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);      // start conversion, with parasite power on at the end
  //delay(1000);          // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {

      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);

    if (cfg == 0x00) raw = raw & ~7;      // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  }
  celsius = (float)raw / 16.0;
  //Serial.print(celsius);
  //Serial.println(" Celsius");
  serialNum = String(addr[7], HEX);
  if (serialNum == "17") oneWire17 = celsius;
  else if (serialNum == "b6") oneWireB6 = celsius;
  else if (serialNum == "d3") oneWireD3 = celsius;

  // If Fahrenheit needed, (Fahrenheit = Celsius * 1.8) + 32;
}

void readDHT11() {
  if (millis() - dhtLastCheck > 10000) {
    chk = DHT.read11(DHT11_PIN);
    switch (chk) {
      case DHTLIB_OK:
        break;
        //case DHTLIB_ERROR_CHECKSUM:
        Serial.print("Checksum error,\t");
        break;
        //case DHTLIB_ERROR_TIMEOUT:
        Serial.print("Time out error,\t");
        break;
      default:
        Serial.print("Unknown error,\t");
        break;
    }
    temp = DHT.temperature;
    umid = DHT.humidity;
    dhtLastCheck = millis();
  }
}

void readTelnetCommand(char c) {
  if (cmd.length() == MAX_CMD_LENGTH) {
    cmd = "";
  }
  cmd += c;
  if (c == '\n' || cmd.length() == limite) {
    parseCommand();
  }
  else {
  }
}

void readSoil() {
  soil = digitalRead(SOIL_PIN);
}

void readPresence() {
  if (digitalRead(PIR_PIN)) {
    pirLastCheck = millis();
    presence = 1;
  } else {
    if (millis() - pirLastCheck > 60000) {
      presence = 0;
    }
  }
}

void parseCommand() {     //Commands received by agent on port 10050 parsing
  if (cmd.equals("")) {  }
  else {
    counter = counter + 1;
    Serial.print(" Tempo: ");
    Serial.print(millis() / 1000);
    Serial.print("\t");
    Serial.print("Cmd: ");
    Serial.print(cmd);
    Serial.print("\t\t");
    Serial.print("Resposta: ");
    // AGENT ping
    if (cmd.equals("p")) {
      server.println("1");
    } // Agent version
    else if (cmd.equals("l")) {
      //Serial.println("Version");
      server.println("Arduino Zabbix Agent 1.0");
      delay(100);
    } // Agent soil humidity
    else if (cmd.equals("q")) {
      readSoil();
      Serial.print(soil);
      server.println(soil);
      // NOT SUPPORTED
    } // Agent air temperature
    else if (cmd.equals("w")) {
      readDHT11();
      Serial.print(temp);
      server.println(temp);
    } // Agent air humidity
    else if (cmd.equals("e")) {
      readDHT11();
      server.println(umid);
      Serial.print(umid);
    } else if (cmd.equals("r")) {
      readOneWire();
      server.println(oneWire17);
      Serial.print(oneWire17);
    } else if (cmd.equals("f")) {
      readOneWire();
      server.println(oneWireB6);
      Serial.print(oneWireB6);
    } else if (cmd.equals("v")) {
      readOneWire();
      server.println(oneWireD3);
      Serial.print(oneWireD3);
    } else if (cmd.equals("t")) {
      server.println(presence);
      Serial.print(presence);
    } else { // Agent error
      //server.print("ZBXDZBX_NOTSUPPORTED");
      //server.print("Error");
    }
    cmd = "";
    Serial.println("");
    client.stop();
  }
}

void loop() {
  client = server.available();
  if (client) {
    if (!connected) {
      Serial.println("Conection not available");
      client.flush();
      connected = true;
      client.stop();
    }
    if (client.available() > 0) {
      Serial.println("Client Available");
      Serial.println("Conection ok");
      int clientread = client.read();
      Serial.print(clientread);
      char charcr = clientread;
      digitalWrite(LED_PIN, HIGH);
      readTelnetCommand(clientread);
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  //Serial.begin(115200);
  pinMode(SOIL_PIN, INPUT);
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.println("Setup");
  delay(1000);
}
