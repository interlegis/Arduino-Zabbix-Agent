#BRANCH UNDER DEVELOPMENT

#Arduino-Zabbix-Agent

Using and Arduino Uno v3 and a Zabbix Server to monitor a data center environment.

It's a Zabbix Agent tested and running successfully.

##Materials:

- Arduino Uno v3
- Ethernet Shield
- Arduino Sensor Shield*
- DHT11 (or similar)
- DS18b20 (or similar)
- PIR (Passive infrared sensor/Motion detector)
- Soil humidity sensor
- Resistors (4.7k and 10k)

###### *A new shield may be built by yourself.

##How to use:

Check pins used.
Upload the code to the Arduino.
Add an item on Zabbix Server.
 - Type: Zabbix agent.
 - Key: The key for each sensor.

##Keys used on Zabbix:

* q - soil humidity
* w - air temperature on DHT11
* e - air humidity on DHT11
* r - air temperature on DS18b20 with serial number ending with 17
* t - air temperature on DS18b20 with serial number ending with b6
* y - motion detector (to be implemented)

##On the code:

*Pins 10, 11, 12 and 13 cannot be used. They are used by the ethernet shield*.

Pin 4 is DHT11.

Pin 5 is DS18b20 (using OneWire)

Pin 6 is PIR

Pin A0 is soil humidity

###Observation:
All temperature are in Celsius.
To convert to Fahrenheit,
`Temp F = 1.8*(Temp C) + 32`

---
Zabbix Agent on Arduino Uno v3
