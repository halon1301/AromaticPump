#include <Arduino.h>
#include "pumpAction.h"
#include "main.h"
#include "ui/ui.h"


unsigned long runPump(int stateMap) {
    unsigned long loc_startTime = millis();
    runstate = true;
    Serial.print("startTime: ");
    Serial.println(loc_startTime);
    digitalWrite(motorPin1, HIGH);
    analogWrite(motorPin2, 64);
    Serial.println("RUN");
    updateUIElemsOn(stateMap);
    startTimeCounter = loc_startTime;
    return loc_startTime;
}

unsigned long stopPump() {
    unsigned long loc_sleepStartTime = millis();
    runstate = false;
    digitalWrite(motorPin1, LOW);
    analogWrite(motorPin2, 0);
    Serial.println("STOP");
    startTimeCounter = 0;
    updateUIElemsOff();
    return loc_sleepStartTime;
}




