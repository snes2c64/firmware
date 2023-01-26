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

#define EEPROM_CONFIG_VERSION 2

// START OF CONFIGURATION

#define MAPCOUNT 8
// clang-format off
const byte defaultMaps[10*MAPCOUNT] = {
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

#define PIN_LED2 13
#define PIN_LED1 12
#define PIN_CLOCK 11
#define PIN_LATCH 10
#define PIN_DATA 9

#define PIN_UP 8
#define PIN_DOWN 6
#define PIN_LEFT 5
#define PIN_RIGHT 2
#define PIN_FIRE 7
#define PIN_FIRE2 3
#define PIN_FIRE3 4

#define MIN_AUTO_FIRE_DELAY 2
#define MAX_AUTO_FIRE_DELAY 64
#define AUTO_FIRE_DELAY_START 4

#define HZ 100

#define EEPROM_OFFSET 0

// END USER CONFIGURATION

#define MODE_DEFAULT 1
#define MODE_START 2
#define MODE_SELECT 4
#define MODE_AUTOFIRESPEED 8

byte disabled_buttons[16];
byte buttons[16];
unsigned long waitTill = 0;
unsigned long debugTiming = 0;
unsigned long modeFallback = 0;
byte newState[7];
bool autofire;
byte autofireCounter;
byte autofireDelay = AUTO_FIRE_DELAY_START;
byte usedmap;
byte mode = MODE_DEFAULT;
byte maps[10 * MAPCOUNT];

#include <EEPROM.h>
#include "version.h"

void writeMapToEEPROM() {
  EEPROM.update(EEPROM_OFFSET, EEPROM_CONFIG_VERSION);
  for (int i = 0; i < MAPCOUNT * 10; i++) {
    EEPROM.update(EEPROM_OFFSET + i + 1, maps[i]);
  }
}

void setMapsToDefault() {
  for (int i = 0; i < MAPCOUNT * 10; i++) {
    maps[i] = defaultMaps[i];
  }
}

void setup() {
  unsigned int i = 0;
  Serial.begin(9600);
  Serial.print("Starting SNES2C64 Firmware v");
  Serial.println(VERSION);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  digitalWrite(PIN_CLOCK, HIGH);
  digitalWrite(PIN_DATA, HIGH);

  controllerRead();

  setMapsToDefault();

  if (buttons[BTN_A] && buttons[BTN_B] && buttons[BTN_X] && buttons[BTN_Y] &&
      buttons[BTN_R]) {
    led1(1);
    led2(1);
    Serial.println("EEPROM reset requested, pleas release all buttons");
    waitForNoButtonPressed();
    Serial.println("To confirm press START, SELECT, L and R");
    while (true) {
      led1((i++ / 512) % 2);
      controllerRead();
      if (buttons[BTN_START] && buttons[BTN_SELECT] && buttons[BTN_L] &&
          buttons[BTN_R] && !buttons[BTN_A] && !buttons[BTN_B] &&
          !buttons[BTN_X] && !buttons[BTN_Y] && !buttons[BTN_UP] &&
          !buttons[BTN_DOWN] && !buttons[BTN_LEFT] && !buttons[BTN_RIGHT]) {
        Serial.println("EEPROM reset confirmed");
        writeMapToEEPROM();
        Serial.println("EEPROM reset done");
        break;
      }
    }
  }
  Serial.print("Checking if EEPROM config is in the right version...");

  if (EEPROM.read(EEPROM_OFFSET) != EEPROM_CONFIG_VERSION) {
    Serial.print("Nope, writing default config to EEPROM");
    writeMapToEEPROM();
    Serial.println("Done");
  } else {
    Serial.println("Yep, loading config.");
    for (int i = 0; i < MAPCOUNT * 10; i++) {
      maps[i] = EEPROM.read(EEPROM_OFFSET + i + 1);
    }
  }
  fixMaps();
  led1(1);
  delay(50);
  led2(1);
  delay(50);
  led1(0);
  delay(50);
  led2(0);
  changeMap(getFirstNonEmptyMap());

  Serial.println("Setup complete.");
}

void handleSerial() {
  if (!Serial.available())
    return;
  char c = Serial.read();
  if (c == 'v') {
    Serial.print("SNES2C64 Firmware v");
    Serial.println(VERSION);
    Serial.print("EEPROM config version: ");
    Serial.println(EEPROM_CONFIG_VERSION);
  }
  if (c == 'd') {
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
    Serial.println("END");
    return;
  }

  if (c == 'u') {

    while (Serial.available() < 11) {
    }
    Serial.print("writing a new Map[");
    int mapNum = Serial.read();
    mapNum = max(0, min(mapNum, MAPCOUNT - 1));
    Serial.print(mapNum, HEX);
    Serial.println("]: ");
    for (int i = 0; i < 10; i++) {
      maps[mapNum * 10 + i] = Serial.read();
      Serial.print(maps[mapNum * 10 + i], HEX);
      Serial.print(" ");
      EEPROM.update(EEPROM_OFFSET + mapNum * 10 + i + 1, maps[mapNum * 10 + i]);
    }
    if (checkMapEmpty(mapNum)) {
      int nextMap = getFirstNonEmptyMap();
      if (nextMap != -1) {
        changeMap(nextMap);
      } else {
        changeMap(fixMaps());
      }
    }
    Serial.println();
    Serial.println("Done");
    return;
  }
  Serial.println("Unknown command");
}

int getFirstNonEmptyMap() {
  for (int i = 0; i < MAPCOUNT; i++) {
    if (!checkMapEmpty(i)) {
      return i;
    }
  }
  return -1;
}

int fixMaps() {
  int nextMap = getFirstNonEmptyMap();
  if (nextMap < 0) {
    setMapsToDefault();
    writeMapToEEPROM();
    return getFirstNonEmptyMap();
  }
}

bool changeMap(int mapNum) {
  if (mapNum < 0 || mapNum >= MAPCOUNT) {
    return false;
  }
  if (checkMapEmpty(mapNum)) {
    return false;
  }
  usedmap = mapNum;
  led1(1);
  while (mapNum-- >= 0) {
    led2(1);
    delay(200);
    led2(0);
    delay(200);
  }
  return true;
}

void loop() {
  handleAutofireFlip();

  clearNewState();

  handleSerial();

  handleDelay();

  controllerRead();
  displayAnyButtonPressed();
  handleReset();

  if (!(mode & MODE_START) && handleSelect())
    return;

  if ((mode & MODE_AUTOFIRESPEED) && handleAutoFireSet())
    return;

  if (handleStart())
    return;

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
  displayAnySignalSend();
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
  if (!buttons[BTN_SELECT] || !buttons[BTN_START])
    return;
  mode = MODE_DEFAULT;
  autofireDelay = AUTO_FIRE_DELAY_START;
  autofire = false;
  autofireCounter = 0;
  for (byte i = 0; i < 16; i++) {
    disabled_buttons[i] = 0;
  }
  for (byte i = 0; i < 10; i++) {
    led1(i % 2);
    delay(100);
    led2(i % 2);
  }
  led1(true);
  led2(true);
  waitForNoButtonPressed();
  led1(false);
  led2(false);
  changeMap(getFirstNonEmptyMap());
}
bool handleSelect() {
  if (mode == MODE_SELECT) {
    if (modeFallback < millis() || buttons[BTN_START]) {
      mode = MODE_DEFAULT;
      waitForNoButtonPressed();
      return true;
    }
    unsigned long diff = modeFallback - millis();
    diff /= 100;
    led2(diff % 2);
    for (byte i = 0; i < 16; i++) {
      if (i == BTN_SELECT || i == BTN_START)
        continue;
      if (!buttons[i])
        continue;
      disabled_buttons[i] = !disabled_buttons[i];
      led1(!disabled_buttons[i]);
      led2(true);
      delay(1500);
      led1(false);
      led2(false);
      mode = MODE_DEFAULT;
      return true;
    }
    return true;
  }
  if (buttons[BTN_SELECT]) {
    mode = MODE_SELECT;
    modeFallback = millis() + 3000;
    return true;
  }
  return false;
}

bool handleAutoFireSet() {
  if (!(mode & MODE_AUTOFIRESPEED)) {
    return false;
  }
  unsigned long nextValueTick = millis();
  bool startreleasedOnce = false;
  do {
    for (byte i = BTN_L; i <= BTN_R; i++) {

      if (buttons[i]) {
        modeFallback = millis() + 3000;
        if (millis() > nextValueTick) {
          autofireDelay += i == BTN_L ? -1 : 1;
          autofireDelay =
              max(min(autofireDelay, MAX_AUTO_FIRE_DELAY), MIN_AUTO_FIRE_DELAY);
          nextValueTick = millis() + 250;
        }
      }
      handleDelay();
      controllerRead();
      if (startreleasedOnce && buttons[BTN_START]) {
        mode = MODE_DEFAULT;
        waitForNoButtonPressed();
        return true;
      }
      if (!buttons[BTN_START]) {
        startreleasedOnce = true;
      }
      handleAutofireFlip();
      led1(autofire);
      led2(autofire);
    }
  } while (modeFallback > millis());
  led2(0);
  led1(0);
  mode = MODE_DEFAULT;

  return true;
}

bool handleStart() {
  if (buttons[BTN_START]) {
    mode = MODE_START;
    modeFallback = millis() + 3000;
    led1(1);
    for (byte i = BTN_L; i <= BTN_R; i++) {

      if (buttons[i]) {
        mode = MODE_START | MODE_AUTOFIRESPEED;
        return handleAutoFireSet();
      }
    }
    return true;
  }
  if (mode & MODE_START) {
    unsigned long diff = modeFallback - millis();
    diff /= 100;
    led1(diff % 2);
    if (modeFallback < millis() || buttons[BTN_SELECT]) {
      mode = MODE_DEFAULT;
      waitForNoButtonPressed();
      return true;
    }
    for (byte i = 0; i < min(8, MAPCOUNT); i++) {
      byte button = i < BTN_SELECT ? i : i + 2;
      if (!buttons[button]) {
        continue;
      }
      waitForNoButtonPressed();
      mode = MODE_DEFAULT;
      if (checkMapEmpty(i)) {
        led2(1);
        led1(1);
        for (byte i = 0; i < 25; i++) {
          led1(i % 2);
          delay(75);
          led2(i % 2);
        }

        delay(1000);
        i = usedmap;
      }
      changeMap(i);
    }
    return true;
  }
  return false;
}

bool checkMapEmpty(byte i) {
  return (maps[i * 10 + 0] == FN_NOP || maps[i * 10 + 0] == FN_AUTO_FIRE) &&
         (maps[i * 10 + 1] == FN_NOP || maps[i * 10 + 1] == FN_AUTO_FIRE) &&
         (maps[i * 10 + 2] == FN_NOP || maps[i * 10 + 2] == FN_AUTO_FIRE) &&
         (maps[i * 10 + 3] == FN_NOP || maps[i * 10 + 3] == FN_AUTO_FIRE) &&
         (maps[i * 10 + 4] == FN_NOP || maps[i * 10 + 4] == FN_AUTO_FIRE) &&
         (maps[i * 10 + 5] == FN_NOP || maps[i * 10 + 5] == FN_AUTO_FIRE) &&
         (maps[i * 10 + 6] == FN_NOP || maps[i * 10 + 6] == FN_AUTO_FIRE) &&
         (maps[i * 10 + 7] == FN_NOP || maps[i * 10 + 7] == FN_AUTO_FIRE) &&
         (maps[i * 10 + 8] == FN_NOP || maps[i * 10 + 8] == FN_AUTO_FIRE) &&
         (maps[i * 10 + 9] == FN_NOP || maps[i * 10 + 9] == FN_AUTO_FIRE);
}

void led1(bool on) { digitalWrite(PIN_LED1, on); }
void led2(bool on) { digitalWrite(PIN_LED2, on); }

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
  led1(pressed);
}
void displayAnySignalSend() {
  bool pressed = false;
  for (byte i = 0; i < 7; i++) {
    pressed = pressed || newState[i];
  }
  led2(pressed);
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
      if (newState[i]) {
      }
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
