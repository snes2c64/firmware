package main

import (
	"flag"
	"fmt"
	"log"
	"regexp"
	"snes2c64gui/pkg/controller"
	"strconv"
)

func main() {
	serialPort := flag.String("serial", "/dev/ttyUSB0", "Serial port to use")
	flag.Parse()

	c, err := controller.NewController(*serialPort)
	if err != nil {
		log.Fatalf("failed to create controller: %v", err)
	}
	defer c.Close()

	args := flag.Args()
	if len(args) > 0 && args[0] == "u" {
		uploadFlags := flag.NewFlagSet("u", flag.ExitOnError)

		mapPosition := uploadFlags.Int("mapPos", -1, "Map positon")
		mapData := uploadFlags.String("map", "", "Map")

		if err := uploadFlags.Parse(args[1:]); err != nil {
			panic(err)
		}

		if *mapPosition == -1 {
			log.Fatalf("mapPos is required")
		}

		const hexRegex = `^[0-9A-Fa-f]{20}$`
		hexRegexCompiled := regexp.MustCompile(hexRegex)
		if !hexRegexCompiled.MatchString(*mapData) {
			log.Fatalf("map must match %s", hexRegex)
		}

		var gamepadMap controller.GamepadMap
		for i := 0; i < 10; i++ {
			btn := (*mapData)[i*2 : i*2+2]

			btnUInt, err := strconv.ParseUint(btn, 16, 8)
			if err != nil {
				log.Fatalf("failed to parse button: %v", err)
			}

			gamepadMap[i] = uint8(btnUInt)
		}

		if err := c.Upload(uint8(*mapPosition), gamepadMap); err != nil {
			log.Fatalf("failed to upload: %v", err)
		}
	}

	maps, err := c.Download()
	if err != nil {
		log.Fatalf("failed to download: %v", err)
	}

	for i, m := range maps {
		fmt.Printf("%d: ", i)
		for _, b := range m {
			fmt.Printf("%02X", b)
		}
		fmt.Println()
	}
}
