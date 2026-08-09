#!/bin/sh

# ------------------------------------------------------------------------------
# Helper Functions
# ------------------------------------------------------------------------------
# Launch process in background only if an instance isn't already running
run() {
    local bin_name
    bin_name="$(basename "$1")"
    if ! pgrep -u "$USER" -x "$bin_name" >/dev/null 2>&1; then
        "$@" &
    fi
}

# ------------------------------------------------------------------------------
# Session & D-Bus Environment Setup
# ------------------------------------------------------------------------------
if command -v dbus-update-activation-environment >/dev/null 2>&1; then
    dbus-update-activation-environment --all
fi

# Polkit authentication agent
if [ -f /usr/libexec/polkit-gnome-authentication-agent-1 ]; then
    run /usr/libexec/polkit-gnome-authentication-agent-1
elif [ -f /usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1 ]; then
    run /usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1
fi

# ------------------------------------------------------------------------------
# Input & Display Settings
# ------------------------------------------------------------------------------
setxkbmap -layout "us,ir"

xset s 600 600
xset dpms 600 600 600

# ------------------------------------------------------------------------------
# Screen Locking & Idle Management
# ------------------------------------------------------------------------------
run xss-lock --transfer-sleep-lock -- slock

run xautolock \
    -time 10 \
    -notify 30 \
    -notifier "brightnessctl -s set 20%" \
    -reset "brightnessctl -r" \
    -locker "xset s activate"

# ------------------------------------------------------------------------------
# Audio Daemon (PipeWire)
# ------------------------------------------------------------------------------
if pgrep -u "$USER" -x "pipewire" >/dev/null 2>&1; then
    pkill -9 -u "$USER" -x "pipewire|pipewire-pulse|wireplumber" 2>/dev/null
    while pgrep -u "$USER" -x "pipewire" >/dev/null 2>&1; do sleep 0.1; done
fi

if [ -n "$XDG_RUNTIME_DIR" ]; then
    rm -f "$XDG_RUNTIME_DIR"/pipewire* "$XDG_RUNTIME_DIR"/pulse*
fi

pipewire &

run picom -b
run dunst

# ------------------------------------------------------------------------------
# System Tray Applets & Applications
# ------------------------------------------------------------------------------
run udiskie --tray
run copyq
run nm-applet
run blueman-applet
run pasystray
run flameshot
run fan
run lock-monitor
run /home/danial/.local/ABDownloadManager/bin/ABDownloadManager --background
run mailspring --password-store="gnome-libsecret" --background
run Telegram -autostart
run /home/danial/.app/Throne/Throne -tray

# Local AI Proxy
run sh -c "cd /home/danial/.npm-global/lib/node_modules/9router/app && PORT=20128 node server.js"

# ------------------------------------------------------------------------------
# Status Bar
# ------------------------------------------------------------------------------
pkill -9 -u "$USER" -x dwmblocks 2>/dev/null
while pgrep -u "$USER" -x dwmblocks >/dev/null 2>&1; do sleep 0.1; done

dwmblocks &
