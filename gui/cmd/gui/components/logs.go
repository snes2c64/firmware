package components

import (
	"fmt"
	"log"
	"time"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/widget"
)

type Logs struct {
	*widget.List

	logs []Log
}

type Log struct {
	timestamp time.Time
	message   string
}

func NewLogs() *Logs {
	logs := []Log{}

	return &Logs{
		List: widget.NewList(
			func() int {
				return len(logs)
			},
			func() fyne.CanvasObject {
				return widget.NewLabel("")
			},
			func(i widget.ListItemID, o fyne.CanvasObject) {
				log.Printf("i: %v, o: %v", i, o)
				o.(*widget.Label).SetText(fmt.Sprintf("%s: %s", logs[i].timestamp.Format("15:04:05"), logs[i].message))
			},
		),
		logs: logs,
	}
}

func (l *Logs) Add(message string) {
	l.logs = append(l.logs, Log{
		timestamp: time.Now(),
		message:   message,
	})
	l.List.Refresh()
}
