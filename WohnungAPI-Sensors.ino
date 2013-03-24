#include <DHT.h>
#include <RCSwitch.h>

#define dhtPin 12
#define rcSwitchPin 3
#define pirPin 2
#define pirInterruptPin 0

#define dhtType DHT11

DHT dht(dhtPin, dhtType);
RCSwitch mySwitch = RCSwitch();

unsigned long previousTemperatureReading = 0;
unsigned long readTemperatureInterval = 20000;

unsigned long motionStarted = 0;
unsigned long motionSendMinimumInterval = 5000;
boolean clearMotionStarted = false;
boolean isMoving = false;

void setup() {

  Serial.begin(9600);

  pinMode(pirPin, INPUT);
  Serial.println("Calibrating PIR Sensor: ");
  for(int i = 0; i < 45; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done.");
  delay(100);
  EIFR = _BV (INTF0);  // clear flag for interrupt 0
  attachInterrupt(pirInterruptPin, motionDetected, RISING); //sets interrupt to RISING, calls motionDetected when interrupted

  mySwitch.enableTransmit(rcSwitchPin);

  dht.begin();
}

void loop() {

  unsigned long currentMillis = millis();
  if(currentMillis - previousTemperatureReading > readTemperatureInterval) {
    previousTemperatureReading = currentMillis;
    readTemperature();  
  }

  if (isMoving) {
    if(currentMillis - motionStarted > motionSendMinimumInterval) {
      log(" SEND MOTION !!!");
      mySwitch.send(111111, 24);
      clearMotionStarted = true;
      motionStarted = currentMillis;
    }
  }
}

void motionStopped() {
  log(" ENTER motionStopped");
  isMoving = false;
  EIFR = _BV (INTF0);  // clear flag for interrupt 0
  detachInterrupt(pirInterruptPin); //detach for movement timing phase
  EIFR = _BV (INTF0);  // clear flag for interrupt 0
  attachInterrupt(pirInterruptPin, motionDetected, RISING); //sets interrupt to RISING, calls motionDetected when interrupted
}

void motionDetected() {
  if (clearMotionStarted || motionStarted == 0) {
    log("clear motion start !");
    motionStarted = millis();
    clearMotionStarted = false;
  }
  log(" ENTER motionDetected");
  isMoving = true;
  EIFR = _BV (INTF0);  // clear flag for interrupt 0
  detachInterrupt(pirInterruptPin); //detach for movement timing phase

  EIFR = _BV (INTF0);  // clear flag for interrupt 0
  attachInterrupt(pirInterruptPin, motionStopped, FALLING);
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

void log(String logMessage) {
  Serial.print(millis()/100);
  Serial.print(" - ");
  Serial.println(logMessage);
}


