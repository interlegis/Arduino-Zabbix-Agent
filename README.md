Arduino-Zabbix-Agent
====================
Using and Arduino Uno v3 and a Zabbix Server to monitor a data center environment.

It's a Zabbix Agent tested and running successfully.
====================
How to use:

Check pins used. (Used pin 5 to DHT11 and analog pin 0 to the soil humidity)
Upload the code to the Arduino.
Add an item on Zabbix. 
 - Type: Zabbix agent. 
 - Key: The cmd set on code. (For example, “t” for temperature)
====================
Zabbix Agent on Arduino Uno v3
