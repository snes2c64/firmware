package views

import (
	"log"
	"strconv"

	fyne "fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"go.bug.st/serial"

	"snes2c64gui/pkg/controller"
)

type UploadView struct {
	Controller *controller.Controller

	StatusBar *widget.Label

	SerialPortsList *widget.Select
	ConnectButton   *widget.Button

	GamepadMapContainer *fyne.Container
	Cols                int
	Rows                int

	UploadButton *widget.Button
}

func NewUploadView() (uv *UploadView) {
	cols := 10
	rows := 8

	statusBar := widget.NewLabel("")
	serialPortsList := widget.NewSelect([]string{}, func(s string) {})
	connectButton := widget.NewButton("Connect", func() {})
	gamepadMapContainer := container.NewGridWithColumns(cols)
	for i := 0; i < rows*cols; i++ {
		gamepadMapContainer.Add(widget.NewEntry())
	}
	uploadButton := widget.NewButton("Upload", func() {})

	defer func() {
		uv.SetStatus("Starting up")

		serialPorts, err := serial.GetPortsList()
		if err != nil {
			log.Fatal(err)
		}

		serialPortsList.Options = serialPorts

		uv.ConnectButton.OnTapped = HandleConnect(uv)
		uv.UploadButton.OnTapped = HandleUpload(uv)
	}()

	return &UploadView{
		StatusBar:           statusBar,
		SerialPortsList:     serialPortsList,
		ConnectButton:       connectButton,
		GamepadMapContainer: gamepadMapContainer,
		Rows:                rows,
		Cols:                cols,
		UploadButton:        uploadButton,
	}
}

func (m *UploadView) Draw(window fyne.Window) {
	window.SetContent(
		container.NewVBox(
			m.StatusBar,
			container.NewHBox(
				m.SerialPortsList,
				m.ConnectButton,
			),
			m.GamepadMapContainer,
			m.UploadButton,
		),
	)
}

func (m *UploadView) SetStatus(status string) {
	m.StatusBar.SetText("Status: " + status)
}

func (m *UploadView) EnableUpload() {
	m.UploadButton.Enable()
}

func HandleConnect(uv *UploadView) func() {
	return func() {
		var err error

		uv.SetStatus("Connecting to /dev/ttyUSB0")

		c, err := controller.NewController("/dev/ttyUSB0")
		if err != nil {
			uv.SetStatus("Failed to connect to /dev/ttyUSB0")
		}
		uv.SetStatus("Connected")

		uv.Controller = c

		gamepadMap, err := uv.Controller.Download()
		if err != nil {
			uv.SetStatus(err.Error())
			return
		}

		for i, v := range gamepadMap {
			for j, v2 := range v {
				uv.GamepadMapContainer.Objects[i*uv.Cols+j].(*widget.Entry).SetText(strconv.Itoa(int(v2)))
			}
		}

		uv.EnableUpload()
	}
}

func HandleUpload(uv *UploadView) func() {
	return func() {
		panic("not implemented")
	}
}
