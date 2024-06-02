//
// Created by falconx on 08/05/24.
//
#include "lib/pumpAction.h"
#include <Arduino.h>
#include "main.h"
#include "lib/remote.h"
#include "M5Unified.h"
#include "../ui/ui.h"

extern void flowUpdateUp() {
    Serial.print("flowUpdateUp: ");
    Serial.println(flowRate);
    if (flowRate < 100) {
        Serial.print("pre: ");
        Serial.println(flowRate);
        flowRate = flowRate + flowRateAdj;
        Serial.print("Post: ");
        Serial.println(flowRate);
        updatePumpFlow();
        updateFlowRateElems();
    }
}

extern void flowUpdateDown() {
    if (flowRate > 0) {
        Serial.print("Pre-down: ");
        Serial.println(flowRate);
        flowRate = flowRate - flowRateAdj;
        Serial.print("Post-Down: ");
        Serial.println(flowRate);
        updatePumpFlow();
        updateFlowRateElems();
    }
}