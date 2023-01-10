package main

import (
	"flag"
	"fmt"
	"regexp"
	"snes2c64gui/pkg/controller"
)

func main() {
	serialPort := flag.String("serial", "/dev/ttyUSB0", "Serial port to use")
	flag.Parse()

	c, err := controller.NewController(*serialPort)
	if err != nil {
		panic(err)
	}
	defer c.Close()

	args := flag.Args()
	if len(args) > 0 && args[0] == "u" {
		mapPosition := flag.Int("mapPos", -1, "Map positon")
		mapData := flag.String("map", "", "Map")

		flag.Parse()

		if *mapPosition == -1 {
			panic("Map position not set")
		}

		const hexRegex = `^[0-9A-Fa-f]{20}$`
		hexRegexCompiled := regexp.MustCompile(hexRegex)
		if !hexRegexCompiled.MatchString(*mapData) {
			panic("Map data not valid")
		}
	}

	maps, err := c.Download()
	if err != nil {
		panic(err)
	}

	fmt.Println(maps)
}
