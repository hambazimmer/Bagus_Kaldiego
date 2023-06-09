#include <Arduino.h>

#define SENSOR 33 // define pin 2 for sensor
#define ACTION 25 // define pin 9 as for ACTION

int objectCount = 0; // variable to track the number of objects detected

void setup() {
  Serial.begin(9600); // setup Serial Monitor to display information
  pinMode(SENSOR, INPUT_PULLUP); // define pin as input for the sensor
  pinMode(ACTION, OUTPUT); // define pin as output for ACTION
  digitalWrite(ACTION, LOW); // turn off the action initially
}

void loop() {
  int L = digitalRead(SENSOR); // read the sensor

  if (L == 0) {
    objectCount++; // increment the object count
    Serial.print("Object detected. Count: ");
    Serial.println(objectCount);

    // Check if the object count is odd or even
    if (objectCount % 2 == 1) {
      digitalWrite(ACTION, HIGH); // turn on the action
      Serial.println("Action: ON");
    } else {
      digitalWrite(ACTION, LOW); // turn off the action
      Serial.println("Action: OFF");
    }

    delay(2000); // delay to avoid counting multiple objects
  }

  delay(100); // add a small delay before the next iteration
}
