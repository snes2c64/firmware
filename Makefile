.PHONY: upload reformat build installadruinodep



upload: installadruinodep
	arduino-cli upload -b arduino:avr:nano:cpu=atmega328old -p /dev/ttyUSB* firmware/
reformat:
	clang-format --sort-includes --style=LLVM firmware/firmware.ino | sponge firmware/firmware.ino
build: installadruinodep
	arduino-cli compile -b arduino:avr:nano:cpu=atmega328old --output-dir dist firmware/
installadruinodep:
	arduino-cli core install arduino:avr
