..
   SNES2C64 documentation master file, created by
   sphinx-quickstart on Thu Jan 19 12:17:03 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

##########
 Overview
##########

This Project is an highly configurable adapter to use a SNES controller
as a Commodore 64 Joystick.

It supports up to 3 fire buttons per Joystick using POTX and POTY as
utilized in games like `MW_ULTRA
<https://www.protovision.games/games/mwultra.php>`_ or anything on that
list over `here <http://wiki.icomp.de/wiki/DB9-Joystick#C64>`_ and can
be tested with `Anykey <https://github.com/T-Pau/Anykey>`_ or `Joyride
<https://github.com/T-Pau/Joyride>`_.

It contains of multiple Parts:

   -  The Hardware, which is basically

      -  a Arduino Nano
      -  2 LED's with resistors
      -  a 9 pin D-Sub connector to connect to the C64
      -  a SNES connector
      -  and last but not least a PCB kindly layouted by @OliverW

   -  Firmware for the Arduino Nano
   -  a cli and gui application to configure the adapter

###############
 Build you Own
###############

You have a few Options here:
   -  you can order you PCB's with the Gerber's over here //TODO
   -  design your own PCB
   -  Just do it using flying wires

******************
 schematics / diy
******************

Schematics are super simple, the designed board uses these over here
//TODO but most of it is flexible. Surely GND from the SNES Connector,
the C64 Connector and the Arduino should be connected, same goes for the
+5V.

All other Pins from the C64 connector and the used Pins from the
SNES-Connector needs to be connected anywhere from D2-D13 on the
Arduino. 2 LED's should be connected to the Arduino as well, one to 2
Pins that are not used up. Preferably one LED should be connected to
PIN13 since it is the one that is used for the LED_BUILTIN. All Pins can
be assigned in the firmware to match up to your wiring.

LED's are optional but are nice to get feedback from the adapter.

*******************
 bill of materials
*******************

Sized are only critical if you use OliverW's Gerbers, otherwise you can use whatever you want.
   -  Arduino Nano

   -  2 LED's either 3mm or //TODO smd size, 2 different colors

   -  2 resistors matching your LED's either 1/4W TH or SMD size //TODO

   -  9 pin D-Sub connector like this //TODO link reichelt

   -  SNES connector Standing. Easily found on aliexpress or Ebay.
      Usually there a 2 different types, standing and angled. While Both
      fit if you use just one Adapter, you can't connect anything in
      Port1 if you use an angled Connector in Port2.

   -  optional: a tactile switch to reset the Arduino if it dosn't come
      with a switch.

   -  optional but recomendend: 3 M3 Screws 5 to 10mm long and 3 M3
      standoffs 5 to 10mm long so that the adapters Connector sits at
      the same height then the C64's

***********************
 flashing the firmware
***********************

You could either open the snes2c64.ino in the Arduino IDE and flash it
from there use the hex file from the releases and flash that in a way
convenient for you. Make sure you have at least set the pins correct if
you don't use the default wiring.

Things you might want to change in Firmware either way:

.. code::

   // START OF CONFIGURATION
   #define MAPCOUNT 8                     // Number of Maps, might be 1-8, further explanation in section Usage //TODO: link
   // clang-format off
   byte maps[10*MAPCOUNT] = {             // Configuration of you Default maps, further explanation in section Usage //TODO: link
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

   // configuration of assigned pins
   #define PIN_LED2 13 // pins for both led'S
   #define PIN_LED1 12
   #define PIN_CLOCK 11 // pins for the SNES connector's clock pin
   #define PIN_LATCH 10 // pins for the SNES connector's latch pin
   #define PIN_DATA 9   // pins for the SNES connector's data pin

   #define PIN_UP 8     // pins for the C64 connector's up pin
   #define PIN_DOWN 6   // pins for the C64 connector's down pin
   #define PIN_LEFT 5   // pins for the C64 connector's left pin
   #define PIN_RIGHT 2  // pins for the C64 connector's right pin
   #define PIN_FIRE 7   // pins for the C64 connector's fire pin
   #define PIN_FIRE2 4  // pins for the C64 connector's fire2 (POTX) pin
   #define PIN_FIRE3 3  // pins for the C64 connector's fire3 (POTY) pin //TODO: check if order is correct

                                    // you are able to adjust autofire speed from the Controller.
   #define MIN_AUTO_FIRE_DELAY 2    // This is the minimum delay between autofire events in cylcles (HZ) lower than 1 makes no sense on a technical level
   #define MAX_AUTO_FIRE_DELAY 64   // This is the maximum delay between autofire events in cylcles (HZ) set it to whatever you want,
                                    // but setting it to high will most likely render autofire useless.
                                    // setting it to 64 with 100HZ will result in 1.64s between autofire events, thats 1.6s on followed by 1.6s off
   #define AUTO_FIRE_DELAY_START 4  // This is the autofire value thats set on startup and reset.

   #define HZ 100  // Frequency the SNES controller is polled and Data is written to the C64
                   // this might work with ridiculously fast values,
                   // but there is no need to go higher then 2 times your screen rate.

   #define EEPROM_OFFSET 0 // Configuration is stored in EEPROM at this offset and is 1+10*MAPCOUNT bytes long
                           // Sometimes bytes in EEPROm are broken so you might want to shift the offset if you got a bad Nano

#######
 Usage
#######

In Normal Mode the D-Pad and the Buttons A B X Y L and R are used for
game play and can be configured freely. you are able to configure up to
8 different button layouts called maps, by hardcoding them into the
firmware before flashing, or by changing them with the configuration
tool afterwards.

Any of these Buttons can be mapt to one, multiple or none of the
following functions:

-  Joystick Up
-  Joystick Down
-  Joystick Left
-  Joystick Right
-  Joystick Fire1
-  Joystick Fire2
-  Joystick Fire3
-  Auto Fire

Yes, that means you can map UP and DOWN to the L Button if you really
need to. A note about Auto fire: Auto fire is not "press the fire button
repeadly" it is "press all configured other buttons repeadly". Meaning:

-  setting the X button just to autofire will do nothing.
-  setting the X button to autofire and Fire1 will result in autofire
   for fire1
-  setting the X button to autofire and UP will result in autofire for
   UP
-  setting the X button to autofire and UP and Fire1 will result in
   autofire for UP and Fire1

In the default configuration there are 2 maps configured:

.. list-table:: Map1 (Default, works fine for most Games)
    :widths: 10 20
    :header-rows: 1

    * - PAD-Button
      - C64 Action
    * - B
      - Fire1
    * - Y
      - Fire1 + Auto Fire
    * - A
      - Fire3
    * - X
      - Fire3 + Auto Fire
    * - L
      - Fire2 + Auto Fire
    * - R
      - Fire2
    * - UP
      - Up
    * - DOWN
      - Down
    * - LEFT
      - Left
    * - RIGHT
      - Right

.. list-table:: Map2
    :widths: 10 20
    :header-rows: 1

    * - PAD-Button
      - C64 Action
    * - B
      - Fire1
    * - Y
      - UP
    * - A
      - Fire1 + Auto Fire
    * - X
      - Fire3
    * - L
      - Fire2
    * - R
      - Fire2
    * - UP
      - Up
    * - DOWN
      - Down
    * - LEFT
      - Left
    * - RIGHT
      - Right

This is useful for platformers and anything that uses UP as Jump, since you can jump using Y and still use the D-Pad for movement.

*****************
Disabling Buttons
*****************

To Prevent accidental jumps you can Disable any Button on the fly.
Just Press SELECT followed my the Button you want to disable.
it is now Disabled, you can reverse is the same way.
Use This for UP with map2 for example in Giana Sisters to have better Jump Control.

**************
Choosing a Map
**************
To choose a Map just press START and the Button of the Map you want to.
Possible Buttons are A, B, X, Y, L, R, UP, DOWN, LEFT, RIGHT.

**********************
Setting Autofire Speed
**********************

You can set the Autofire Speed by Pressing and HOLDING START and pressing L or R for faster or slower.

*********************
Resetting the Adapter
*********************
If you cet stuck somehow by choosing a wrong map and / or disabling buttons (START and SELECT can't be disabled) you can reset the adapter bei either
- unplugging it
- pressing START and SELECT together
- pressing the reset button

####################
Reading LED Feedback
####################

***********
Starting Up
***********

After Starting up in a fast pace LED1 turns on Followed by LED2 and both turning off in the same sequence.
Your Adapter is ready to use now.

**********
Normal Use
**********

In normal mode LED1 is on if any of the gameplay buttons (A B X Y L R or DPAD) is pressed.
LED2 is on if any action for the C64 is on.

This means, using the default map1:
- while holding down the B Button LED1 is on and LED2 is on.
- while holding down the Y Button LED1 is on and LED2 is flashes since it's autofireing Fire1.
- while holding down the Y and B Buttons LED1 and LED2 are on since there is always at least one actiopn for the C64 triggerd.
- with a disabled Y Button while Y is pressed LED 1 is on and LED2 is off.


**************
Choosing a Map
**************
When choosing a map by Pressing START, LED1 starts flashing for 2 seconds and turns of after.
While LED2 is flashing you can choose a map by pressing the Button of the map you want to choose.
Map to Button Mapping is as follows:

.. list-table:: Map to Button
    :widths: 10 20
    :header-rows: 1

    * - 1
      - B
    * - 2
      - Y
    * - 3
      - UP
    * - 4
      - DOWN
    * - 5
      - LEFT
    * - 6
      - RIGHT
    * - 7
      - A
    * - 8
      - X

When a map is chosen, LED1 stays on while LED2 blinks the number of the map you chose.
then Both LEDs turn off, the map is active and normal mode is active again.

When a Map is chosen that is empty (no buttons mapped or only mapped to autofire without an actual action),
the Firmware refuses to activate that map. It show that by rapidly flashing LED1 and LED2 for 2 seconds in a alternating pattern.
Afterwards it show the previous map as if you chose that map.
eg:
assuming default configuration (map 1 and 2 are set, map 3 is empty), you are on map 2 and choosing map 3:
LED1 and LED2 start flashing in an alternating pattern, followed by LED1 turning on and LED2 blinking 2 times.

If START is pressed accidentally and you don't want to wait 2 seconds you can stop choosing with SELECT.

***********************
Button disable toggling
***********************

When disabling or enabling again a Button, LED2 starts flashing for 2 seconds and turns of after.
While LED1 is flashing you can choose a button to disable by pressing the Button you want to disable.

When a button is pressed LED2 stays on for about 2 seconds.
In that 2 Seconds LED1 displays the new status of that button.
ON for enabled and OFF for disabled.

If SELECT is pressed accidentally and you don't want to wait 2 seconds you can stop choosing with START.

**********************
Setting Autofire Speed
**********************

You can set the autofire speed by pressing and holding START and pressing L or R for faster or slower.
while holding L or R LED1 is on and LED2 is flashing the speed.


*********************
Resetting the Adapter
*********************

You can reset the adapter basically in 2 Ways:
- unplugging it or using the Reset on the Arduino
- pressing START and SELECT together

When resetting with a power cycle or the actual reset the adapter will behave as described in "Starting Up".

When resetting with START and SELECT LEDS 1 and 2 will flash in an alternate pattern for about 1 second and then turn off.
The firmware is not actually now but brought back to the state it was in when the adapter was started.

If using the SOFT-reset via START and SELECT will result in a different behavior then a HARD-reset via the reset button you probably have found a bug, please report it.


