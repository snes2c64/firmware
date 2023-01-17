package views

import (
	"fmt"

	fyne "fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"

	"snes2c64gui/cmd/gui/components"
	"snes2c64gui/pkg/controller"
)

type UploadView struct {
	Controller *controller.Controller

	StatusBar *widget.Label
	LogsView  *components.Logs

	ConnectModalButton fyne.CanvasObject
	Gamepad            *components.Gamepad

	UploadButton *widget.Button
}

func NewUploadView(window fyne.Window) (uv *UploadView) {
	cols := 10
	rows := 8

	statusBar := widget.NewLabel("")

	connectModal := components.NewConnectModal(window.Canvas(), func(port string) {
		handleConnect(uv, uv.Controller, port)()
	})

	gamepad := components.NewGamepad()

	gamepadMapContainer := container.NewGridWithColumns(cols)
	for i := 0; i < rows*cols; i++ {
		gamepadMapContainer.Add(widget.NewEntry())
	}
	uploadButton := widget.NewButton("Upload", func() {})
	uploadButton.Disable()

	logsView := components.NewLogs()

	defer func() {
		uv.SetStatus("Starting up")

		uv.UploadButton.OnTapped = handleUpload(uv)
	}()

	return &UploadView{
		StatusBar:          statusBar,
		ConnectModalButton: connectModal,
		Gamepad:            gamepad,
		LogsView:           logsView,
		UploadButton:       uploadButton,
	}
}

func (uv *UploadView) Draw(window fyne.Window) {
	window.SetContent(
		container.NewHBox(
			container.NewVBox(
				uv.ConnectModalButton,
				uv.Gamepad.Container,
				uv.UploadButton,
			),
			uv.StatusBar,
		),
	)
}

func (uv *UploadView) SetStatus(status string) {
	uv.StatusBar.SetText("Status: " + status)
	uv.LogsView.Add(status)
}

func (uv *UploadView) EnableUpload() {
	uv.UploadButton.Enable()
}

func (uv *UploadView) Upload() {
	uv.SetStatus("Uploading")

	err := uv.Controller.Upload(uint8(uv.Gamepad.SelectedMap()), uv.Gamepad.Map().Map())
	if err != nil {
		uv.SetStatus(err.Error())
		return
	}

	uv.SetStatus("Upload complete")
}

func (uv *UploadView) Download() {
	uv.SetStatus("Downloading")

	gamepadMaps, err := uv.Controller.Download()
	if err != nil {
		uv.SetStatus(err.Error())
		return
	}

	uv.Gamepad.SetMaps(gamepadMaps)

	uv.SetStatus("Download complete")

}

func handleConnect(uv *UploadView, c *controller.Controller, port string) func() {
	return func() {
		var err error

		uv.SetStatus(fmt.Sprintf("Connecting to %s", port))

		c, err := controller.NewController(port)
		if err != nil {
			uv.SetStatus(fmt.Sprintf("Error connecting to %s: %v", port, err))
			return
		}
		uv.Controller = c

		uv.SetStatus("Connected")

		uv.Download()

		uv.Gamepad.SetSelectedMap(0)
		uv.Gamepad.Enable()
		uv.EnableUpload()
	}
}

func handleUpload(uv *UploadView) func() {
	return func() {
		uv.Upload()
		uv.Download()
	}
}
