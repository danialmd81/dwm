#!/bin/sh

# Helper function to prevent launching multiple instances of a daemon
run() {
	if ! pgrep -f "$1" >/dev/null; then
		"$@" &
	fi
}

# ------------------------------------------------------------------------------
# Session & D-Bus Environment Setup
# ------------------------------------------------------------------------------
# 1. Update D-Bus environment synchronously (NO run / NO &)
if command -v dbus-update-activation-environment >/dev/null 2>&1; then
	dbus-update-activation-environment DISPLAY XAUTHORITY SSH_AUTH_SOCK
fi

# 2. Secrets management and keyring daemon (NO run / MUST USE eval)
if command -v gnome-keyring-daemon >/dev/null 2>&1; then
	eval $(gnome-keyring-daemon --start --components=secrets,ssh)
	export SSH_AUTH_SOCK
fi

# 3. Polkit authentication agent
if [ -f /usr/libexec/polkit-gnome-authentication-agent-1 ]; then
	run /usr/libexec/polkit-gnome-authentication-agent-1
elif [ -f /usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1 ]; then
	run /usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1
fi

# Screen lock service
run xss-lock --transfer-sleep-lock -- i3lock --nofork

# Keyboard Layouts
setxkbmap -layout "us,ir" -option "grp:win_space_toggle" &

# ------------------------------------------------------------------------------
# Desktop Daemons & System Tray Applets
# ------------------------------------------------------------------------------
run pipewire
run picom -b
run dunst
run copyq
run nm-applet
run blueman-applet
run pasystray
run /home/danial/.local/ABDownloadManager/bin/ABDownloadManager --background
run mailspring --password-store="gnome-libsecret" --background
run Telegram -autostart
run /home/danial/.app/Throne/Throne -tray

# ------------------------------------------------------------------------------
# Status Bar
# ------------------------------------------------------------------------------
# Gracefully kill existing dwmblocks and wait for process termination
pkill -9 -x dwmblocks 2>/dev/null
while pgrep -u $UID -x dwmblocks >/dev/null; do sleep 0.1; done

# Launch dwmblocks
dwmblocks &
