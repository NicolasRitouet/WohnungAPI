#include <DHT.h>
#include <RCSwitch.h>

#define dhtPin 12
#define rcSwitchPin 4
#define pirPin 2
#define pirInterruptPin 0

#define dhtType DHT11

DHT dht(dhtPin, dhtType);
RCSwitch mySwitch = RCSwitch();

unsigned long previousTemperatureReading = 0;
unsigned long readTemperatureInterval = 2000;

boolean isMoving = false; //has there been movement in the last 5 seconds
boolean sendMoving = false; //has there been a message about movement sent
unsigned long startTime; //start time in millis of timer
boolean timerStarted = false; //has the timer been started

void setup() {

  Serial.begin(9600);

  pinMode(pirPin, INPUT);
  Serial.println("Calibrating PIR Sensor: ");
  for(int i = 0; i < 30; i++) {
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
  //nothing happens until interrupt
  while (isMoving) {
    if(sendMoving) { //only send message once
      Serial.println("Motion Detected!"); //prints motion once
      sendMoving = false;
    }
    if (digitalRead(pirPin) == LOW && lowTimer() == true) { //if no movement and over five seconds on timer
      Serial.println("Motion Stopped.");
      attachInterrupt(pirInterruptPin, motionDetected, RISING);//reattach interrupt
    }else {
       Serial.println("Motion Continues.");
    }
  }
}

void motionDetected() {
  Serial.println("ENTER motionDetected");
  isMoving = true;
  sendMoving = true;
  EIFR = _BV (INTF0);  // clear flag for interrupt 0
  detachInterrupt(pirInterruptPin); //detach for movement timing phase
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
  } else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
    mySwitch.send(t*100, 24);
  }
}

boolean lowTimer() {
  Serial.println("ENTER lowTimer");

  while (digitalRead(pirPin) == LOW) {
    if (timerStarted == false) { //start timer once
      startTime = millis();
      timerStarted = true;
    } else if (timerStarted == true && (millis()-startTime) > 5000) { //if the timer has gone for five seconds
      isMoving = false;
      timerStarted = false;
      Serial.println("EXIT lowTimer and reset isMoving");
      return true;
    }
  }
  //if movement occurs during the timing, exit and reset timer
  timerStarted = false;
  Serial.println("EXIT lowTimer");
  return false;
}

