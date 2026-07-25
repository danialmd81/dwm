#!/bin/sh

run() {
	if ! pgrep -f "$1" >/dev/null; then
		"$@" &
	fi
}

# Execute all .desktop entries in ~/.config/autostart/ and /etc/xdg/autostart/
run dex -a -s ~/.config/autostart/

# Desktop services
run picom -b
run dunst
run copyq
run nm-applet
run blueman-applet
run pasystray

# Status bar (must run before or alongside dwm)
run dwmblocks
