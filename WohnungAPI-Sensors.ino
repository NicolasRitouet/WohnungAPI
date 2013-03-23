#include "DHT.h"
#include <RCSwitch.h>

#define dhtPin 12
#define rcSwitchPin 4
#define pirPin 13

#define dhtType DHT11


DHT dht(dhtPin, dhtType);
RCSwitch mySwitch = RCSwitch();

boolean sensorActive = false;
boolean previousSensorState = false;

void setup() {
  Serial.begin(9600);

  pinMode(pirPin, INPUT);
  mySwitch.enableTransmit(rcSwitchPin);
  dht.begin();
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
    mySwitch.send("000000000001010100010001");
  } 
  else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
    transmitTemperature(t);
  }
  delay(2000);
  if(digitalRead(pirPin) == HIGH)
  {
    sensorActive = true;
  }
  if(digitalRead(pirPin) == LOW)
  {
    sensorActive = false;
  }
  if(sensorActive != previousSensorState)
  {
    if(sensorActive == true)
    {
      previousSensorState = sensorActive;
      Serial.println("Mouvement");
      mySwitch.send(12345, 24);
    } 
    else {
      previousSensorState = sensorActive;
      Serial.println("Mouvement fini");
      mySwitch.send(54321, 24);
    }
    delay(50);
  }
}

void transmitTemperature(float temperature) {
  mySwitch.send(temperature*100, 24);
}

