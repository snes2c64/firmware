#!/usr/bin/env php
How this generally works:

Hardware:
get an Arduino, a Nano is fine.
get a Female D-Sub 9 Pin connector.
Get a SNES Controller Socket, or just snip cour Controller Cable.

Connect the GND from the C64 and SNES to the Arduinos GND, do the same with the 5V.

Connect all other Pins from the SNES and the C64 to any IO-Pins on the Arduino, prefferable skip the ones that are used for Serial Communication and the internal LED, thats 0,1 and 13 on most models including the nano.

Software:
Now open config.ino and tell it which pin is what.

While you are there you can chance the autofire settings and the HZ setting.
You can read what that mens down below.


Now think about how you would like your pat to behave, which button should do what.
You are free to configure the D-Pad, and the buttons A B X Y L R.

You can have up to 8 different profiles, each with its own settings.
see maps.ino, you can add to const byte maps, there should be 10 items for every profile / map.
they represent the buttons B Y Up Down Left Right A X L R and can be set any action possible for the C64, thats:
    - FN_NOP, which is just nothing
    - FN_UP, which is the up arrow
    - FN_DOWN, which is the down arrow
    - FN_LEFT, which is the left arrow
    - FN_RIGHT, which is the right arrow
    - FN_FIRE, which is the fire button
    - FN_FIRE2, which is the fire button 2
    - FN_FIRE3, which is the fire button 3
    - FN_AUTO_FIRE, which autofire autofires the action it is combined with.

Yes, you can all the actions to a single button bay bitwise oring them.
Want your B Button to press UP and all 3 Firebuttons? just set it to FN_UP | FN_FIRE | FN_FIRE2 | FN_FIRE3
Want your A to Autofire Fire2? just set it to FN_FIRE2 | FN_AUTO_FIRE

Make sure to set MAPCOUNT to the amount of your maps.
The First map is the map that is loaded on startup.
Now you can upload things to your arduino, either by opening snes2c64.ino in the arduino ide, or just with make upload if you have platformio installed.

Usage:
Connect the SNES-Controller to tha matching Socket and the D-Sub to the C64.

The D-Pad and the Buttons A B X Y L R should do what you have configured them in the default map.

You can change settings using the Start and Select buttons.

Using the select button you can temporary disable or re enable and of the 10 free Buttons.
by first pressing the button to act on, then the select button and let go of them in any order.
This Button is now disabled or re enabled.
This is usefully if for example if you don't like jumping with UP, you might have a mapping with FN_UP an UP and Y.
Depending on the game one of them might be useless.

Using the Startbutton following by any of the D-Pad Directions or A B X Y you can choose your configured maps.
The ordering is:
0 => B
1 => Y
2 => Up
3 => Down
4 => Left
5 => Right
6 => A
7 => X

Pressing Start + L makes Autofire faster
Pressing Start + R makes Autofire slower
all within the bounds of MIN_AUTO_FIRE_DELAY and MAX_AUTO_FIRE_DELAY.
More on that below.
Pressing Start + Select (not Select + Start) resets every temporary setting, that means:
the First mapping is active again
all buttons are re enabled
Autofire is set to the default value.


Below is a documentation of the current configuration with explanations of the settings.


<?php
$config = [];
$maps = [];
$mode = null;

$fns = [
    1 => "UP",
    2 => "DOWN",
    4 => "LEFT",
    8 => "RIGHT",
    16 => "FIRE",
    32 => "FIRE2",
    64 => "FIRE3",
];
$buttons=[
        "B","Y","UP","DOWN","LEFT","RIGHT","A","X","L","R"
];
while ($line = fgets(STDIN)) {
    $line = trim($line);
    if (0 === strpos($line, "#")) {
        continue;
    }
    if (0 === strpos($line, ":")) {
        $mode = substr($line, 1);
        continue;
    }
    switch ($mode) {
        case "define":
            $parts = explode("=", $line, 2);
            $config[trim($parts[0])] = $parts[1];
            break;
        case "maps":
            $parts = explode(" ", $line);
            $mapNum = $parts[0];
            $button = $buttons[$parts[1]];
            $actions = hexdec($parts[2]);
            $autoFire = $parts[3];

            if (!isset($maps[$mapNum])) {
                $maps[$mapNum] = [];
            }
            if (!isset($maps[$mapNum][$button])) {
                $maps[$mapNum][$button] = [
                    "actions" => [],
                    "autoFire" => $autoFire,
                ];
            }
            for ($i = 7; $i >= 0; $i--) {
                $pos = pow(2, $i);
                if ($actions >= $pos) {
                    $actions -= $pos;
                    $maps[$mapNum][$button]["actions"][] = $fns[$pos];
                }
            }
            break;
        default:
            echo "unknown mode $mode\n";
            die(1);
    }
}


$configDesc=[
"PIN_LED"=>"Arduino pin the internal or External LED is connected to",
"PIN_CLOCK"=>"Arduino pin the SNES controller's clock is connected to",
"PIN_DATA"=>"Arduino pin the SNES controller's data is connected to",
"PIN_LATCH"=>"Arduino pin the SNES controller's latch is connected to",
"PIN_UP"=>"Arduino pin the C64's up pin is connected to",
"PIN_DOWN"=>"Arduino pin the C64's down pin is connected to",
"PIN_LEFT"=>"Arduino pin the C64's left pin is connected to",
"PIN_RIGHT"=>"Arduino pin the C64's right pin is connected to",
"PIN_FIRE"=>"Arduino pin the C64's fire pin is connected to",
"PIN_FIRE2"=>"Arduino pin the C64's fire2/POTX pin is connected to",
"PIN_FIRE3"=>"Arduino pin the C64's fire3/POTY pin is connected to",
"HZ"=>"The Frequency of the Arduino's main loop, to low will get you input lag, to high will get you dropped inputs. Your screen frequency x1 or x2 should be fine, eg 50/100Hz for PAL, 60/120 for NTSC",
"MIN_AUTO_FIRE_DELAY"=>"Minimal delay between autofire events in main loops, this is a multiple of HZ",
"MAX_AUTO_FIRE_DELAY"=>"Maximal delay between autofire events in main loops, this is a multiple of HZ",
"AUTO_FIRE_DELAY_START"=>"The default value for the delay between autofire events in main loops",

"MAPCOUNT"=>"The amount of the configured Button maps. There should be at least one, up to 8 are possible",
    ];



$mapButtons=['B','Y','UP','DOWN','LEFT','RIGHT','A','X'];

foreach($config as $k=>$v){
    echo "$k = $v:\n  $configDesc[$k] \n";
}

foreach($maps as $mapNum=>$map) {
    echo "\nMap $mapNum (".$mapButtons[$mapNum]."):\n";
    foreach ($map as $button => $actions) {
        echo "  Button ".str_pad($button,6," ").": ". implode(", ", $actions["actions"]) , $actions["autoFire"]?" with Autofire":'',"\n";

    }
}
?>


