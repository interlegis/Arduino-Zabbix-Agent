//*****************************************************************************************
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

#define MAX_CMD_LENGTH 25
#define DHT11_PIN 5        // DHT11 pin
#define SOLO_PIN A0            // Soil humidity sensor pin

//  Network settings
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0xE3, 0x5B };
IPAddress ip(10, 1, 2, 235);
IPAddress gateway(10, 1, 2, 254);
IPAddress subnet(255, 255, 255, 0);

EthernetServer server = EthernetServer(10050); //Zabbix Agent port //Zabbix agent port
EthernetClient client;
dht DHT;

String cmd;               //FOR ZABBIX COMMAND
int counter = 1;          // For testing 
int solo = 0;             // Soil humidity
int limite = 1;           // Command size. Using 1 for better performance.
double temp = 0;          // Temperature
double umid = 0;          // Humidity
boolean connected = false;

void setup() {
  Serial.begin(115200);
  pinMode(SOLO_PIN,INPUT);
  Ethernet.begin(mac, ip, gateway, subnet);  
  server.begin();
  
  int chk = DHT.read11(DHT11_PIN);
  switch (chk) {
    case DHTLIB_OK:  
      break;
    case DHTLIB_ERROR_CHECKSUM: 
      //Serial.print("Checksum error,\t"); 
      break;
    case DHTLIB_ERROR_TIMEOUT: 
      //Serial.print("Time out error,\t"); 
      break;
    default: 
      //Serial.print("Unknown error,\t"); 
      break;
  }
}

void loop() {
  delay(500);
  temp = DHT.temperature;
  umid = DHT.humidity;
  delay(100);
  solo = digitalRead(SOLO_PIN);
  delay(100);
  
  client = server.available();
  if (client == true) {
    if (!connected) {
//      Serial.println("Conection not available");
      client.flush();
      connected = true;
      client.stop();
    }
    if (client.available() > 0) {
//      Serial.println("Client Available");
//      Serial.println("Conection ok");
      int clientread = client.read();
//      Serial.print(clientread);
      char charcr = clientread;
      readTelnetCommand(clientread);
    }
  }
}

void readTelnetCommand(char c) {
  if(cmd.length() == MAX_CMD_LENGTH) {
    cmd = "";
  }
  cmd += c;
  if(c == '\n' || cmd.length() == limite) {
      parseCommand();
  }
  else {
  }
}

void parseCommand() {     //Commands recieved by agent on port 10050 parsing
  counter = counter + 1;
  // AGENT ping
  Serial.print("Tempo: ");
  Serial.print(millis()/1000);
  Serial.print("\t");
  Serial.print("Cmd: ");
  Serial.print(cmd);
  Serial.print("\t\t");
  Serial.print("Resposta: ");
  if(cmd.substring(0,4) == "ping") {
    server.println("1");
    client.stop();
   } // Agent version
   else if(cmd.equals("v")) {
    //Serial.println("Version");
    server.println("Arduino Zabbix Agent 1.0");
    delay(100);
    client.stop();
  } // Agent soil humidity
  else if(cmd.equals("s")) {
    Serial.println(solo);
    server.println(solo);
    client.stop();  
// NOT SUPPORTED
  } // Agent air temperature
  else if(cmd.equals("t")) {
    Serial.println(temp);
    server.println(temp);
    client.stop();
  } // Agent air humidity
 else if(cmd.equals("u")) {
    server.println(umid);
    Serial.println(umid);
    client.stop();
  } else { // Agent error
    // server.println("ZBXDZBX_NOTSUPPORTED");
    server.println("Error");
    client.stop();
  }
  cmd = "";
}
