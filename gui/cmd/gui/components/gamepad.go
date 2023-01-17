package components

import (
	"fmt"
	"snes2c64gui/pkg/controller"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/widget"
)

type GamepadMap struct {
	*fyne.Container
}

var c64buttons = []string{
	"UP",
	"DOWN",
	"LEFT",
	"RIGHT",
	"FIRE1",
	"FIRE2",
	"FIRE3",
	"AUTOFIRE",
}

func NewGamepadMap() *GamepadMap {
	keyCount := 10

	gamepadMap := &GamepadMap{
		container.NewHBox(),
	}

	for i := 0; i < keyCount; i++ {
		buttonsContainer := container.NewVBox()
		for _, button := range c64buttons {
			check := widget.NewCheck(button, func(bool) {})
			check.Disable()
			buttonsContainer.Add(check)

		}

		gamepadMap.Container.Add(
			buttonsContainer,
		)
	}

	return gamepadMap
}

func (m *GamepadMap) Enable() {
	for _, number := range m.Container.Objects {
		for _, button := range number.(*fyne.Container).Objects {
			button.(*widget.Check).Enable()
		}
	}
}

func (m *GamepadMap) Disable() {
	for _, number := range m.Container.Objects {
		for _, button := range number.(*fyne.Container).Objects {
			button.(*widget.Check).Disable()
		}
	}
}

func (m *Gamepad) Enable() {
	m.gamepadMapView.Enable()

	radioGroup := m.Container.Objects[0].(*widget.RadioGroup)
	radioGroup.Enable()
}

func (m *Gamepad) Disable() {
	m.gamepadMapView.Disable()

	radioGroup := m.Container.Objects[0].(*widget.RadioGroup)
	radioGroup.Disable()
}

func (m *GamepadMap) SetMap(gamepadMap controller.GamepadMap) {
	for i, number := range gamepadMap {
		for j := 0; j < 8; j++ {
			if int(number)&pow2(j) != 0 {
				m.Container.Objects[i].(*fyne.Container).Objects[j].(*widget.Check).SetChecked(true)
			} else {
				m.Container.Objects[i].(*fyne.Container).Objects[j].(*widget.Check).SetChecked(false)
			}
		}
	}
}

func (m *GamepadMap) Map() controller.GamepadMap {
	var gamepadMap controller.GamepadMap

	for i, number := range m.Container.Objects {
		for j, button := range number.(*fyne.Container).Objects {
			if button.(*widget.Check).Checked {
				gamepadMap[i] |= (uint8(pow2(j)))
			}
		}
	}

	return gamepadMap
}

func pow2(n int) int {
	if n == 0 {
		return 1
	}

	return 2 * pow2(n-1)
}

type Gamepad struct {
	*fyne.Container

	gamepadMapView *GamepadMap

	selectedMap int
	GamepadMaps []controller.GamepadMap
}

func NewGamepad() *Gamepad {
	mapCount := 8

	gamepadMap := NewGamepadMap()

	gamepad := &Gamepad{
		container.NewVBox(
			widget.NewRadioGroup([]string{}, func(string) {}),
			gamepadMap.Container,
		),
		gamepadMap,
		0,
		make([]controller.GamepadMap, mapCount),
	}

	var options []string
	for i := 0; i < mapCount; i++ {
		options = append(options, fmt.Sprintf("Map %d", i))
	}

	radioGroup := gamepad.Container.Objects[0].(*widget.RadioGroup)
	radioGroup.Options = options
	radioGroup.Horizontal = true
	radioGroup.SetSelected("Map 0")
	radioGroup.Disable()
	radioGroup.OnChanged = func(value string) {
		gamepad.SetSelectedMap(int(value[4]) - 48)
	}

	return gamepad
}

func (m *Gamepad) SetMaps(gamepadMaps []controller.GamepadMap) {
	m.GamepadMaps = gamepadMaps
}

func (m *Gamepad) SetSelectedMap(mapIndex int) {
	m.selectedMap = mapIndex

	radioGroup := m.Container.Objects[0].(*widget.RadioGroup)
	radioGroup.SetSelected(fmt.Sprintf("Map %d", mapIndex))

	m.gamepadMapView.SetMap(m.GamepadMaps[mapIndex])
}

func (m *Gamepad) SelectedMap() int {
	return m.selectedMap
}

func (m *Gamepad) Map() *GamepadMap {
	return m.gamepadMapView
}

func (m *Gamepad) handleMapSelect(mapIndex int) func() {
	return func() {
		m.SetSelectedMap(mapIndex)
	}
}
