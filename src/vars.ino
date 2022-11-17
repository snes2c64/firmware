#ifndef VARS_INO_
#define VARS_INO_
byte disabled_buttons[16];
byte buttons[16];
unsigned long waitTill = 0;
unsigned long debugTiming = 0;

byte newState[7];
bool autofire;
byte autofireCounter;
byte autofireDelay = AUTO_FIRE_DELAY_START;
byte usedmap;
#endif