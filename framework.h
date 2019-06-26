/**
   Cruise Control software
   Written by @Tobiaqs
*/

#include "helpers.h"

// TPS1 value the actuator should be steering towards
unsigned int actuatorTarget = 0;
unsigned int actuatorTargetAfterStationary = 0;

// Tacho vars
unsigned long tachoTimePrevHigh = 0;
bool tachoPrevLoopHigh = false;
unsigned long tachoAvgDutyCycle = 0;
byte tachoDutyCycleCount = 0;
bool rpmJustUpdated = false;
unsigned int rpm = 0;
unsigned int rpmPrev = 0;
unsigned int rpmTarget = 0;

bool active = false;

// Button vars
byte buttonStateNewlyPressed = 0;
byte buttonStateRaw = 0;

void measureButtons() {
  byte buttonStateRawPrev = buttonStateRaw;
  buttonStateRaw = 0;
  if (!digitalRead(BTN1)) {
    buttonStateRaw |= B1;
  }
  if (!digitalRead(BTN2)) {
    buttonStateRaw |= B10;
  }
  if (!digitalRead(BTN3)) {
    buttonStateRaw |= B100;
  }
  if (!digitalRead(BTN4)) {
    buttonStateRaw |= B1000;
  }

  buttonStateNewlyPressed = (buttonStateRaw ^ buttonStateRawPrev) & buttonStateRaw;
}

void registerDutyCycle(unsigned int duration) {
  // Add this duty cycle duration to the average duty cycle variable
  tachoAvgDutyCycle += duration;
  // Keep track of the number of duty cycles we've passed
  tachoDutyCycleCount ++;

  // Once the number of duty cycles has reached rpm / 300 (at most 15), we take the average
  // and convert it to RPM.
  if (tachoDutyCycleCount == 20) {
    tachoAvgDutyCycle /= tachoDutyCycleCount;
    
    rpmPrev = rpm;
    rpm = convertDutyCycleToRpm(tachoAvgDutyCycle);
    rpmJustUpdated = true;
    
    tachoAvgDutyCycle = 0;
    tachoDutyCycleCount = 0;
  }
}

void measureRPM() {
  long now = micros();

  // Overflow detection
  if (tachoTimePrevHigh > now) {
    // Skip one duty cycle. This doesn't reset the averaging mechanism!
    tachoTimePrevHigh = 0;
    tachoPrevLoopHigh = true;
  }

  // When RPM reading is in progress, but result will be lower than 500 RPM (STALL_RPM),
  // reset and prepare for new reading.
  if (tachoTimePrevHigh != 0 && now - tachoTimePrevHigh > CONVERSION_FACTOR / 500) {
    rpm = 0;
    tachoTimePrevHigh = 0;
    tachoPrevLoopHigh = false;
    tachoAvgDutyCycle = 0;
    tachoDutyCycleCount = 0;
  }
  
  bool tachoSignal = isTachoSignalHigh();
  
  if (tachoSignal && !tachoPrevLoopHigh) {
    tachoPrevLoopHigh = true;
 
    if (tachoTimePrevHigh == 0) {
      tachoTimePrevHigh = now;
    } else {
      registerDutyCycle(now - tachoTimePrevHigh);

      tachoTimePrevHigh = now;
    }
  } else if (!tachoSignal) {
    tachoPrevLoopHigh = false;
  }
}

void reset() {
  rpmTarget = 0;
  actuatorTarget = 0;
  actuatorTargetAfterStationary = 0;
}

bool abortIfNecessary() {
  if (actuatorTarget != 0 && isBrakePressed()) {
    reset();
    return true;
  }
  return false;
}

void matchThrottle() {
  Serial.println("matchThrottle called");
  actuatorTargetAfterStationary = getThrottlePosition();

  if (actuatorTargetAfterStationary < MIN_THROTTLE_POSITION || actuatorTargetAfterStationary > MAX_THROTTLE_POSITION) {
    actuatorTargetAfterStationary = 0;
  }

  Serial.print("actuatorTargetAfterStationary = ");
  Serial.println(actuatorTargetAfterStationary);
}

unsigned int getModulateDownSpeed(unsigned int throttlePosition) {
  return 25;
}

unsigned int getModulateUpSpeed(signed int throttlePosition) {
  return min(255, 100 + (2 * max(0, throttlePosition - 30) / 20));
}

void modulateThrottle() {
  bool stationary = isStationary();
  
  if (actuatorTargetAfterStationary != 0 && stationary) {
    actuatorTarget = actuatorTargetAfterStationary;
    actuatorTargetAfterStationary = 0;
    active = true;
    Serial.print("active = true");
    Serial.print("  actuatorTarget = ");
    Serial.println(actuatorTarget);
  }
  
  if (actuatorTarget == 0) {
    return;
  }
  
  unsigned int throttlePosition = getThrottlePosition();

  Serial.print("actuatorTarget = ");
  Serial.print(actuatorTarget);
  Serial.print("  throttlePosition = ");
  Serial.print(throttlePosition);

  if (isDifferenceSignificant(throttlePosition, actuatorTarget, SIGNIFICANCE_THROTTLE_POSITION)) {
    signed int diff = throttlePosition - actuatorTarget;
    Serial.print("  diff = ");
    Serial.print(diff);
    unsigned int speed = 0;
    if (diff > 0) {
      speed = getModulateDownSpeed(throttlePosition);
      controlActuator(less, speed);
    } else {
      speed = getModulateUpSpeed(throttlePosition);
      controlActuator(more, speed);
    }
    Serial.print("  speed = ");
    Serial.println(speed);
  } else {
    if (actuatorDirection == less) {
      controlActuator(more, 255);
      delay(5);
    }
    Serial.println("  diff = insignificant");
    controlActuator(halt, 0);
  }
}

/*
unsigned long lastModulation = 0;

void modulateSpeed() {
  if (rpmTarget == 0) {
    return;
  }

  unsigned long now = millis();

  if (lastModulation <= now - MODULATE_DELAY) {
    unsigned int throttlePosition = getThrottlePosition();
    
    if (isDifferenceSignificant(rpm, rpmTarget, SIGNIFICANCE_RPM)) {
      signed int diff = rpm - rpmTarget;
      
      if (diff < 0 && throttlePosition + STEP_THROTTLE_POSITION <= MAX_THROTTLE_POSITION) {
        actuatorTarget = throttlePosition + STEP_THROTTLE_POSITION;
        lastModulation = now;
      } else if (diff > 0 && throttlePosition - STEP_THROTTLE_POSITION >= MIN_THROTTLE_POSITION) {
        actuatorTarget = throttlePosition - STEP_THROTTLE_POSITION;
        lastModulation = now;
      }
    } else {
      actuatorTarget = throttlePosition;
      lastModulation = now;
    }
  }
}
*/
