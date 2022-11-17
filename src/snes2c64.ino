
#include "./config.ino"
#include "./init.ino"
#include "./maps.ino"
#include "./vars.ino"
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
