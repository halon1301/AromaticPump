#include <Arduino.h>
#include "pumpAction.h"
#include "main.h"
#include "ui/ui.h"


unsigned long runPump(int stateMap) {
    unsigned long loc_startTime = millis();
    unsigned int loc_flowRate = flowRate;
    runstate = true;
    int flowNumber = calcFlowRateNumber(loc_flowRate);
    digitalWrite(motorPin1, LOW);
    analogWrite(motorPin2, flowNumber); // 0 is 0%, 255 is 100%
    Serial.println("RUN");
    updateUIElemsOn(stateMap);
    if (initialStartTime == 0) {
        initialStartTime = loc_startTime;
    }
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
    runElapsedTime = (loc_sleepStartTime - startTime) + runElapsedTime;
    return loc_sleepStartTime;
}

void updatePumpFlow() {
    int flowNumber = calcFlowRateNumber(flowRate);
    digitalWrite(motorPin1, LOW);
    analogWrite(motorPin2, flowNumber); // 0 is 0%, 255 is 100%
}

int calcFlowRateNumber(unsigned int loc_flowRate) {
    int flowNumber;
    if (loc_flowRate == 100) {
        flowNumber = 0.99 * 256;
    } else {
        flowNumber = (loc_flowRate * 0.01) * 256;
    }
    return flowNumber;
}