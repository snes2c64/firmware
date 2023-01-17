package components

import (
	"fmt"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
	"go.bug.st/serial"
)

type ConnectModal struct {
	*widget.Button

	Modal *widget.PopUp

	OnConnect func(port string)
}

func NewConnectModal(parent fyne.Canvas, onConnect func(port string)) fyne.CanvasObject {
	serialPorts, err := serial.GetPortsList()
	if err != nil {
		// TODO: Handle error in ui
		panic(fmt.Sprintf("Error getting serial ports: %v", err))
	}
	portsList := widget.NewSelect(serialPorts, func(s string) {})

	modal := widget.NewModalPopUp(nil, parent)
	connect := widget.NewButton("Connect", func() {
		modal.Hide()
		onConnect(portsList.Selected)
	})

	open := widget.NewButton("Connect", func() {
		modal.Show()
	})

	modal.Content = container.NewVBox(
		widget.NewLabel("Select a serial port"),
		portsList,
		connect,
	)

	return &ConnectModal{
		Button: open,
		Modal:  modal,
	}

}
