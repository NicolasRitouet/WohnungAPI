/**
 * ATTINY85 - Temperature sensor/Motion detector/Data transmitter
 *
 * PIN Configuration:
 * ATtiny Pin 1 = (RESET) N/U                       ATtiny Pin 8 = VCC (2.7-5.5V)
 * ATtiny Pin 2 = (D3) IR OUTPUT PIN                ATtiny Pin 7 = D2/INT0/SCK on GPIO
 * ATtiny Pin 3 = (D4) to LED1                      ATtiny Pin 6 = (D1) Piezo
 * ATtiny Pin 4 = GND                               ATtiny Pin 5 = D0/SDA on GPIO
 *
 * @author Nicolas Ritouet
 * TODO: Read temperature/humidity, detect motion, send data
 */
int ledPin = 3;
int buttonPin = 1;
int buttonIntPin = 0;

unsigned long previousTemperatureReading = 0;
unsigned long readTemperatureInterval = 20000;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  attachInterrupt(buttonIntPin, lightOn, CHANGE);
}

void loop() {
  // unsigned long currentMillis = millis();
  // if(currentMillis - previousTemperatureReading > readTemperatureInterval) {
  //   previousTemperatureReading = currentMillis;
  //   readTemperature();  
  // }
}

void lightOn() {
  if (digitalRead(ledPin) == HIGH) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
}

void readTemperature() {
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
    Serial.print(millis());
    Serial.print(" Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
    mySwitch.send(t*100, 24);
  }
}
