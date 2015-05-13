#Arduino-Zabbix-Agent
====================
Using and Arduino Uno v3 and a Zabbix Server to monitor a data center environment.

It's a Zabbix Agent tested and running successfully.

====================
##How to use:

Check pins used.
Upload the code to the Arduino.
Add an item on Zabbix. 
 - Type: Zabbix agent. 
 - Key: The cmd set on code.

====================
##On the code:

Pins 10, 11, 12 and 13 cannot be used. They are used by the ethernet shield.
Pin 4 is DHT11.
Pin 5 is DS18b20 using OneWire
Pin A0 is soil humidity

##Keys used on Zabbix:

* s - soil humidity
* t - air temperature on DHT11
* u - air humidity on DHT11
* x - air temperature on DS18b20 with serial number ending with 17
* z - air temperature on DS18b20 with serial number ending with b6

###Observation:
All temperature are in Celsius.
To convert to Fahrenheit,
`Temp F = 1.8*(Temp C) + 32`

====================
Zabbix Agent on Arduino Uno v3
