Wohnung API
===========

My own home automation system, based on arduino.  
Plan is:  
A) in each room, a wireless module that transmits temperature and motion detection.  
Each module will be composed of:
- a attiny 85
- a dht22 (temperature and humidity sensor) (or LM35 ?)
- a pir motion detection sensor (HC-SR501)
- a wireless transmitter (433mhz)

B) one central arduino to collect the data and share them through an API.  
This module is composed of:
- an arduino UNO
- an ethernet Shield
- a wireless receiver (433Mhz)
- a lipo charger (in case of power outage)
- a lipo battery

Trello board: https://trello.com/board/wohnung-api
