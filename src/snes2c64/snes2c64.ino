#define FN_NOP 0
#define FN_UP 1
#define FN_DOWN 2
#define FN_LEFT 4
#define FN_RIGHT 8
#define FN_FIRE 16
#define FN_FIRE2 32
#define FN_FIRE3 64
#define FN_AUTO_FIRE 128

#define BTN_B 0
#define BTN_Y 1
#define BTN_SELECT 2
#define BTN_START 3
#define BTN_UP 4
#define BTN_DOWN 5
#define BTN_LEFT 6
#define BTN_RIGHT 7
#define BTN_A 8
#define BTN_X 9
#define BTN_L 10
#define BTN_R 11

#define EEPROM_CONFIG_VERSION 1


// START OF CONFIGURATION

#define MAPCOUNT 8
// clang-format off
byte maps[8*MAPCOUNT] = {
                    /* B     */ FN_FIRE,
                    /* Y     */ FN_FIRE | FN_AUTO_FIRE,

                    /* ️️UP    */ FN_UP,
                    /* DOWN  */ FN_DOWN,
                    /* LEFT  */ FN_LEFT,
                    /* RIGHT */ FN_RIGHT,
                    /* A     */ FN_FIRE2,
                    /* X     */ FN_FIRE2 | FN_AUTO_FIRE,
                    /* L     */ FN_FIRE3 | FN_AUTO_FIRE,
                    /* R     */ FN_FIRE3,

                    /* B     */ FN_FIRE,
                    /* Y     */ FN_UP,
                    /* ️️UP    */ FN_UP,
                    /* DOWN  */ FN_DOWN,
                    /* LEFT  */ FN_LEFT,
                    /* RIGHT */ FN_RIGHT,
                    /* A     */ FN_FIRE | FN_AUTO_FIRE,
                    /* X     */ FN_FIRE2,
                    /* L     */ FN_FIRE3,
                    /* R     */ FN_FIRE3,
                    };
// clang-format on


#define PIN_LED 13
#define PIN_CLOCK 11
#define PIN_LATCH 10
#define PIN_DATA 9

#define PIN_UP 8
#define PIN_DOWN 6
#define PIN_LEFT 5
#define PIN_RIGHT 2
#define PIN_FIRE 7
#define PIN_FIRE2 4
#define PIN_FIRE3 3

#define MIN_AUTO_FIRE_DELAY 2
#define MAX_AUTO_FIRE_DELAY 64
#define AUTO_FIRE_DELAY_START 4

#define HZ 100

// END USER CONFIGURATION





byte disabled_buttons[16];
byte buttons[16];
unsigned long waitTill = 0;
unsigned long debugTiming = 0;

byte newState[7];
bool autofire;
byte autofireCounter;
byte autofireDelay = AUTO_FIRE_DELAY_START;
byte usedmap;



#include <EEPROM.h>

void setup() {
  Serial.begin(9600);
  //Serial.println("Starting...");
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  digitalWrite(PIN_CLOCK, HIGH);
  digitalWrite(PIN_DATA, HIGH);
  //Serial.print("Checking if eeprom config is in the right version...");

 //if (EEPROM.read(0) != EEPROM_CONFIG_VERSION) {
 //  Serial.print("Nope, resetting to default config");
 //  EEPROM.write(0, EEPROM_CONFIG_VERSION);
 //  for (int i = 0; i < MAPCOUNT * 10; i++) {
 //    EEPROM.write(i + 1, maps[i]);
 //  }
 //  Serial.println("Done");
 //} else {
 //  Serial.println("Yep, loading config.");
 //  for (int i = 0; i < MAPCOUNT * 10; i++) {
 //    maps[i] = EEPROM.read(i + 1);
 //  }

 //  Serial.println("Setup complete.");
 //}
}

void handleSerial() {
  if (!Serial.available())
    return;
  char c = Serial.read();
  if (c == 'd') {
    //Serial.println("showing all maps:");
    Serial.println("START");
    for (int i = 0; i < MAPCOUNT * 10; i++) {
      if (maps[i] < 0x10) {
        Serial.print("0");
      }
      Serial.print(maps[i], HEX);
      if (i % 10 == 9) {
        Serial.println();
      } else {
        Serial.print(" ");
      }
    }
    Serial.print("END");
    return;
  }

  if (c == 'u') {

    if (Serial.available() >= 10) {
      Serial.println("writing a new Map[");
      int mapNum = max(0, min(Serial.read(), MAPCOUNT));
      Serial.print(mapNum);
      Serial.println("]: ");
      for (int i = 0; i < 10; i++) {
        maps[mapNum * 10 + i] = Serial.read();
        Serial.print(maps[mapNum * 10 + i], HEX);
        // EEPROM.write(i + 1, maps[i]);
      }
      Serial.println("Done");
    }
    return;
  }
  Serial.println("Unknown command");
}

void loop() {
  handleAutofireFlip();

  clearNewState();

  handleSerial();

  handleDelay();

  controllerRead();

  if (buttons[BTN_SELECT] && buttons[BTN_START]) {
    handleReset();
    return;
  }
  if (buttons[BTN_SELECT]) {
    handleSelect();
    return;
  }

  if (buttons[BTN_START]) {
    handleStart();
    return;
  }

  for (byte i = 0; i < 16; i++) {
    if (disabled_buttons[i]) {
      buttons[i] = 0;
    }
  }

  for (byte i = 0; i < 10; i++) {
    byte button = i >= BTN_SELECT ? i + 2 : i;
    byte fn = maps[i + 10 * usedmap];
    byte hasAutofire = fn & FN_AUTO_FIRE;
    fn = fn & ~FN_AUTO_FIRE;

    action(fn, buttons[button], hasAutofire);
  }

  sendNewState();

  displayAnyButtonPressed();
}

void clearNewState() {
  for (byte i = 0; i < 7; i++) {
    newState[i] = 0;
  }
}

void handleDelay() {
  while (waitTill > millis()) {
  }
  waitTill = millis() + 1000 / HZ;
}

void handleAutofireFlip() {
  if (autofireCounter++ >= autofireDelay) {
    autofire = !autofire;
    autofireCounter = 0;
  }
}

void handleReset() {
  autofireDelay = AUTO_FIRE_DELAY_START;
  autofire = false;
  autofireCounter = 0;
  digitalWrite(PIN_LED, LOW);
  for (byte i = 0; i < 16; i++) {
    disabled_buttons[i] = 0;
  }
  for (byte i = 0; i < 20; i++) {
    led(i % 2);
    delay(100);
  }
}
void handleSelect() {
  for (byte i = 0; i < 16; i++) {
    if (i == BTN_SELECT || i == BTN_START) {
      continue;
    }
    if (buttons[i]) {
      disabled_buttons[i] = !disabled_buttons[i];
    }
  }
  led(1);
  waitForNoButtonPressed();
  led(0);
}
void handleStart() {

  for (byte i = BTN_L; i <= BTN_R; i++) {
    if (buttons[i]) {
      autofireDelay += i == BTN_L ? -1 : 1;
      do {
        handleDelay();
        controllerRead();
        handleAutofireFlip();
        led(autofire);
      } while (buttons[i]);
      led(0);
      autofireDelay =
          max(min(autofireDelay, MAX_AUTO_FIRE_DELAY), MIN_AUTO_FIRE_DELAY);
      return;
    }
  }

  for (byte i = 0; i < min(8, MAPCOUNT); i++) {
    byte button = i < BTN_SELECT ? i : i + 2;
    if (!buttons[button]) {
      continue;
    }
    usedmap = i;
    waitForNoButtonPressed();
  }
}

void led(bool on) { digitalWrite(PIN_LED, on); }

void waitForNoButtonPressed() {
  bool pressed;
  do {
    pressed = false;
    handleDelay();
    controllerRead();
    for (byte i = 0; i < 16; i++) {
      pressed = pressed || buttons[i];
    }
  } while (pressed);
}

void displayAnyButtonPressed() {
  bool pressed = false;
  for (byte i = 0; i < 16; i++) {
    pressed = pressed || buttons[i];
  }
  led(pressed);
}

void controllerRead() {
  digitalWrite(PIN_LATCH, HIGH);
  // there should be a 12µs delay between latch high and latch low, but since
  // digitaslWrite takes about 5µs this is fine
  delayMicroseconds(7);
  digitalWrite(PIN_LATCH, LOW);

  // there are just 12 buttons, but we have to read 16 bits in for the
  // controller to work properly.
  // we just read them in normally knowing well they always be 0, but this is
  // simpler and faster than branching of

  for (byte i = 0; i < 16; i++) {
    // there should be a 6µs delay between click high and low.
    // since a digital write takes about 5µs this is fine without delay
    digitalWrite(PIN_CLOCK, LOW);

    buttons[i] = !digitalRead(PIN_DATA);
    digitalWrite(PIN_CLOCK, HIGH);
  }
}

void action(byte fn, bool pressed, bool isautofire) {
  for (byte i = 0; i < 7; i++) {
    if (fn & (1 << i)) {
      newState[i] = newState[i] || (pressed && (!isautofire || autofire));
    }
  }
}

void sendNewState() {
  pinMode(PIN_UP, newState[0]);
  pinMode(PIN_DOWN, newState[1]);
  pinMode(PIN_LEFT, newState[2]);
  pinMode(PIN_RIGHT, newState[3]);
  pinMode(PIN_FIRE, newState[4]);

  pinMode(PIN_FIRE2, newState[5]);
  digitalWrite(PIN_FIRE2, newState[5]);

  pinMode(PIN_FIRE3, newState[6]);
  digitalWrite(PIN_FIRE3, newState[6]);
}
