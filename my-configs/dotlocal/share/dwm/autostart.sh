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

# # ------------------------------------------------------------------------------
# # Input & Display Settings
# # ------------------------------------------------------------------------------
# setxkbmap -layout "us,ir"

# ------------------------------------------------------------------------------
# Screen Locking & Idle Management
# ------------------------------------------------------------------------------
# Helper script to restore brightness upon unlocking
LOCK_WRAPPER='sh -c "xsecurelock; brightnessctl -r"'

# # xss-lock listens for logind/sleep events and manual lock triggers
# run xss-lock --transfer-sleep-lock -- sh -c "$LOCK_WRAPPER"

# # Idle configuration:
# run xautolock -time 20 -notify 10 -notifier "/home/danial/.local/share/dwm/notify-dim.sh" -locker "$LOCK_WRAPPER" -corners "----"

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

# ------------------------------------------------------------------------------
# System Tray Applets & Applications
# ------------------------------------------------------------------------------
run picom -b
run dunst

# ------------------------------------------------------------------------------
# System Tray Applets & Applications
# ------------------------------------------------------------------------------
run nm-applet
run pasystray
run udiskie --tray
run blueman-applet
run copyq
run flameshot
run monitor-fan
run monitor-caps-num-lock
run /home/danial/.app/Throne/Throne -tray
run mailspring --password-store="gnome-libsecret" --background
run Telegram -autostart
run /home/danial/.local/ABDownloadManager/bin/ABDownloadManager --background

# Local AI Proxy
run sh -c "node /home/danial/.npm-global/lib/node_modules/9router/cli.js serve --no-open --tray"
# run sh -c "node /home/danial/.npm-global/lib/node_modules/omniroute/bin/omniroute.mjs serve --no-open --tray"

# ------------------------------------------------------------------------------
# Status Bar
# ------------------------------------------------------------------------------
pkill -9 -u "$USER" -x dwmblocks 2>/dev/null
while pgrep -u "$USER" -x dwmblocks >/dev/null 2>&1; do sleep 0.1; done

dwmblocks &
