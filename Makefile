.PHONY: upload reformat build installadruinodep release_patch release_minor release_major tag pre_release post_release firmware/version.h



upload: installadruinodep firmware/version.h
	arduino-cli upload -b arduino:avr:nano:cpu=atmega328old -p /dev/ttyUSB* firmware/
reformat:
	clang-format --sort-includes --style=LLVM firmware/firmware.ino | sponge firmware/firmware.ino
build: installadruinodep firmware/version.h
	arduino-cli compile -b arduino:avr:nano:cpu=atmega328old --output-dir dist firmware/
	./renameBuildFiles
installadruinodep:
	arduino-cli core install arduino:avr
versioner:
	wget -q "https://raw.githubusercontent.com/schorsch3000/versioner/master/versioner" -O versioner
	chmod +x versioner

firmware/version.h: versioner .version
	echo "#define VERSION \"$(shell ./getVersionName)\"" > firmware/version.h



tag:
	git tag -a $(shell ./versioner -i 0.1.0) -m "Release $(shell ./versioner -i 0.1.0)"


.version: versioner
	./versioner -i 0.1.0

pre_release: .version


post_release:
	$(MAKE) firmware/version.h
	$(MAKE) build
	git commit -am "Release $(shell ./versioner -i 0.1.0)"
	$(MAKE) tag


release_patch: pre_release
	./versioner  -p
	$(MAKE) post_release

release_minor: pre_release
	./versioner  -m
	$(MAKE) post_release

release_major: pre_release
	./versioner  -M
	$(MAKE) post_release

