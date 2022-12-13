.PHONY: upload reformat build configExtractor

upload:
	platformio run -t upload
reformat:
	clang-format --sort-includes --style=LLVM src/snes2c64/snes2c64.ino | sponge src/snes2c64/snes2c64.ino

build:
	platformio run

configExtractor:
	gcc genDoc.c
	./a.out | ./config.php
	rm a.out