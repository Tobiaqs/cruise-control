/**
   Cruise Control software
   Written by @Tobiaqs
*/

#include "framework.h"

void setup() {
  pinMode(TPS1, INPUT);
  pinMode(TPS2, INPUT);
  pinMode(IDL, INPUT_PULLUP);
  pinMode(BRK, INPUT);
  pinMode(TCH, INPUT);
  pinMode(DIR1, OUTPUT);
  pinMode(SPD, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);

  controlActuator(halt, 0);

  Serial.begin(9600);
}

void loop() {
  
  measureRPM();

  Serial.print("TPS1: ");
  Serial.println(analogRead(TPS1));

  measureButtons();
  handleButtons();

  abortIfNecessary();

  modulateThrottle();
}

void handleButtons() {
  if (buttonStateNewlyPressed == 0) {
    return;
  }

  if ((buttonStateNewlyPressed & B1) != 0) {
    matchThrottle();
  }

  if ((buttonStateNewlyPressed & B10) != 0) {
    releaseThrottle();
    reset();
  }

  if (actuatorTarget != 0) {
    if ((buttonStateNewlyPressed & B100) != 0) {
      actuatorTarget = max(MIN_THROTTLE_POSITION, actuatorTarget - STEP_THROTTLE_POSITION);
    }
  
    if ((buttonStateNewlyPressed & B1000) != 0) {
      actuatorTarget = min(MAX_THROTTLE_POSITION, actuatorTarget + STEP_THROTTLE_POSITION);
    }
  }

  /* if (rpmTarget != 0) {
    if ((buttonStateNewlyPressed & B100) != 0) {
      log("Setting rpmTarget to " + rpmTarget);
      rpmTarget = max(MIN_RPM, rpmTarget - STEP_RPM);
    }
  
    if ((buttonStateNewlyPressed & B1000) != 0) {
      log("Setting targetRpm to " + rpmTarget);
      rpmTarget += STEP_RPM;
    }
  }*/
}
