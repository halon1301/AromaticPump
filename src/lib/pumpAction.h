#include <Arduino.h>

const int motorPin1 = 27;
const int motorPin2 = 19;

extern unsigned long runPump(int stateMap);

extern unsigned long stopPump();

extern void updatePumpFlow();

int calcFlowRateNumber(unsigned int loc_flowRate);
