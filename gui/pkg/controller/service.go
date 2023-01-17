package controller

import (
	"fmt"
	"io"
	"strconv"
	"strings"

	"go.bug.st/serial"
)

const (
	SetupCompleteMsg = "Setup complete."

	DownloadCmd         = "d"
	DownloadStartMsg    = "START"
	DownloadCompleteMsg = "END"

	UploadCmd     = "u"
	UploadDoneMsg = "Done"
)

type GamepadMap [10]uint8

type Controller struct {
	port io.ReadWriteCloser
}

func NewController(p string) (*Controller, error) {
	port, err := serial.Open(p, &serial.Mode{})
	if err != nil {
		return nil, fmt.Errorf("failed to open port: %w", err)
	}

	if _, err := readUntil(port, SetupCompleteMsg); err != nil {
		return nil, fmt.Errorf("failed to read from port: %w", err)
	}

	return &Controller{
		port,
	}, nil
}

func (c *Controller) Close() error {
	if err := c.port.Close(); err != nil {
		return fmt.Errorf("failed to close port: %w", err)
	}

	return nil
}

func (c *Controller) Download() (g []GamepadMap, err error) {
	if _, err := c.port.Write([]byte(DownloadCmd)); err != nil {
		return nil, fmt.Errorf("failed to write to port: %w", err)
	}

	m, err := readUntil(c.port, DownloadCompleteMsg)
	if err != nil {
		return nil, fmt.Errorf("failed to read from port: %w", err)
	}

	lines := m[strings.Index(m, DownloadStartMsg)+len(DownloadStartMsg)+1 : strings.LastIndex(m, DownloadCompleteMsg)]
	for _, line := range strings.Split(lines[1:len(lines)-2], "\r\n") {
		var gamepadMap GamepadMap

		for i, button := range strings.Split(line, " ") {
			functions, err := strconv.ParseUint(button, 16, 8)
			if err != nil {
				return nil, fmt.Errorf("failed to parse button: %w", err)
			}

			f := uint8(functions)
			gamepadMap[i] = f
		}

		g = append(g, gamepadMap)
	}

	return g, nil
}

func (c *Controller) Upload(n uint8, g GamepadMap) error {
	if _, err := c.port.Write([]byte(UploadCmd)); err != nil {
		return fmt.Errorf("failed to write to port: %w", err)
	}

	b := make([]byte, 1)
	b[0] = n

	if _, err := c.port.Write(b); err != nil {
		return fmt.Errorf("failed to write to port: %w", err)
	}

	for _, button := range g {
		b := make([]byte, 1)
		b[0] = button

		if _, err := c.port.Write(b); err != nil {
			return fmt.Errorf("failed to write to port: %w", err)
		}
	}

	if _, err := readUntil(c.port, UploadDoneMsg); err != nil {
		return fmt.Errorf("failed to read from port: %w", err)
	}

	return nil
}

func readUntil(port io.ReadWriteCloser, msg string) (string, error) {
	var content strings.Builder

	for {
		buf := make([]byte, 128)
		n, err := port.Read(buf)
		if err != nil {
			return "", fmt.Errorf("failed to read from port: %w", err)
		}
		if n == 0 {
			break
		}
		content.Write(buf[:n])

		if strings.Contains(content.String(), msg) {
			break
		}
	}

	return content.String(), nil
}
