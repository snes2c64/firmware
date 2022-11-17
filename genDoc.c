typedef unsigned char byte; // 8-bit unsigned entity.

#include "./src/config.ino"
#include "./src/init.ino"

#include "./src/maps.ino"

#include <stdio.h>

int main(void) {
  printf(":define\n");
  printf("PIN_LED = %d\n", PIN_LED);
  printf("PIN_CLOCK = %d\n", PIN_CLOCK);
  printf("PIN_DATA = %d\n", PIN_DATA);
  printf("PIN_LATCH = %d\n", PIN_LATCH);

  printf("PIN_UP = %d\n", PIN_UP);
  printf("PIN_DOWN = %d\n", PIN_DOWN);
  printf("PIN_LEFT = %d\n", PIN_LEFT);
  printf("PIN_RIGHT = %d\n", PIN_RIGHT);
  printf("PIN_FIRE = %d\n", PIN_FIRE);
  printf("PIN_FIRE2 = %d\n", PIN_FIRE2);
  printf("PIN_FIRE3 = %d\n", PIN_FIRE3);

  printf("MIN_AUTO_FIRE_DELAY = %d\n", MIN_AUTO_FIRE_DELAY);
  printf("MAX_AUTO_FIRE_DELAY = %d\n", MAX_AUTO_FIRE_DELAY);
  printf("AUTO_FIRE_DELAY_START = %d\n", AUTO_FIRE_DELAY_START);
  printf("HZ = %d\n", HZ);

  printf("MAPCOUNT = %d\n", MAPCOUNT);
  printf(":maps\n# a b cc d\n# where a is the map number, b the button, c the function and d shows if it's autofire or not");
  for (int i = 0; i < MAPCOUNT; i++) {

    for (int j = 0; j < 10; j++) {
      printf("  %d %d %02x %d\n",i,  j, 0b01111111 & maps[i * 10 + j],  maps[i * 10 + j] >> 7);
    }
  }
}
