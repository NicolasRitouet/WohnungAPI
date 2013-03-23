Wohnung API
===========

My own home automation system, based on arduino.
Plan is:
- in each room, a module that transmit temperature and motion detection. This module will be composed of:
  - a attiny 85
	- a dht22 (temperature and humidity sensor)
	- a pir motion detection sensor (HC-SR501)
	- a wireless transmitter (433mhz) 

- one central arduino to collect the data and share them through an API. This module is composed of:
	- an arduino UNO
	- an ethernet Shield
	- a wireless receiver (433Mhz)
	- a lipo charger (in case of power outage)
	- a lipo battery

TODO
====
- Add watchdot sleep in the sensor module
- Add API (when ethernet shield arrives)
- Document the protocole between Sensor modules and the central
- Receive data from a wireless door opening detector
