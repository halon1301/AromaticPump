#include <Arduino.h>

const int motorPin1 = 27;
const int motorPin2 = 19;

extern unsigned long runPump();

extern unsigned long stopPump();

extern void updateUIElemsOff();

extern void updateUIElemsOn();

extern void updateUIElems10sOff();

extern void updateUIElems10sOn();

