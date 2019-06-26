/**
   Cruise Control software
   Written by @Tobiaqs
*/

#include "constants.h"

direction actuatorDirection = halt;

void log(const char* str) {
  Serial.println(str);
}

// converts duty cycle duration in microseconds to RPM
// (note: only works for durations >= 458 microseconds
// due to type limitation)
unsigned int convertDutyCycleToRpm(unsigned long duration) {
  return CONVERSION_FACTOR / duration;
}

// converts RPM to duty cycle duration in microseconds
// (note: only works for RPMs >= 458 due to type limitation)
unsigned int convertRpmToDutyCycle(unsigned int rpm) {
  return CONVERSION_FACTOR / rpm;
}

bool isDifferenceSignificant(unsigned int a, unsigned int b, unsigned int maxDifference) {
  if (a == b) {
    return false;
  } else {
    unsigned int difference;
    if (a > b) {
      difference = a - b;
    } else {
      difference = b - a;
    }
    return difference > maxDifference;
  }
}

bool isTachoSignalHigh() {
  return analogRead(TCH) >= THRESHOLD_TCH_HIGH;
}

bool isBrakePressed() {
  return analogRead(BRK) >= THRESHOLD_BRK_HIGH;
}

bool isStationary() {
  return !digitalRead(IDL);
}

unsigned int getThrottlePosition() {
  return analogRead(TPS1);
}

void controlActuator(direction dir, byte speed) {
  actuatorDirection = dir;
  
  if (dir == halt) {
    analogWrite(3, 0);
    digitalWrite(2, LOW);
    digitalWrite(4, LOW);
  } else if (dir == less) {
    analogWrite(3, speed);
    digitalWrite(2, LOW);
    digitalWrite(4, HIGH);
  } else if (dir == more) {
    analogWrite(3, speed);
    digitalWrite(2, HIGH);
    digitalWrite(4, LOW);
  }
}

void releaseThrottle() {
  if (isStationary()) {
    return;
  }
  
  controlActuator(less, RELEASE_SPEED);

  while (!isStationary());

  controlActuator(more, 20);

  delay(20);

  controlActuator(halt, 0);
}
