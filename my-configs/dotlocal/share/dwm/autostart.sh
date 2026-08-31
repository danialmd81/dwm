#!/bin/sh

# ------------------------------------------------------------------------------
# Helper Functions
# ------------------------------------------------------------------------------
run() {
    bin_name="$(basename "$1")"
    if ! pgrep -u "$USER" -x "$bin_name" >/dev/null 2>&1; then
        "$@" &
    fi
}

wait_for_stop() {
    name="$1"
    max="${2:-50}" # default ~5s timeout
    i=0
    while pgrep -u "$USER" -x "$name" >/dev/null 2>&1; do
        i=$((i + 1))
        if [ "$i" -ge "$max" ]; then
            echo "autostart: timed out waiting for '$name' to stop" >&2
            break
        fi
        sleep 0.1
    done
}

# ------------------------------------------------------------------------------
# Input & Display Settings
# ------------------------------------------------------------------------------
# Keyboard layout
setxkbmap -layout "us,ir" \
    -option "grp:win_space_toggle"

# ------------------------------------------------------------------------------
# Polkit authentication agent
# ------------------------------------------------------------------------------
for agent in \
    /usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1 \
    /usr/libexec/polkit-gnome-authentication-agent-1 \
    /usr/lib/polkit-kde-authentication-agent-1 \
    /usr/bin/lxqt-policykit-agent \
    /usr/bin/lxpolkit; do
    if [ -x "$agent" ]; then
        run "$agent"
        break
    fi
done

# ------------------------------------------------------------------------------
# Screen Locking & Idle Management
# ------------------------------------------------------------------------------
# LOCK_WRAPPER='sh -c "xsecurelock; brightnessctl -r"'
# run xss-lock --transfer-sleep-lock -- sh -c "$LOCK_WRAPPER"
# run xautolock -time 20 -notify 10 \
#     -notifier "/home/danial/.local/share/dwm/notify-dim.sh" \
#     -locker "$LOCK_WRAPPER" -corners "----"

# ------------------------------------------------------------------------------
# Audio Daemon (PipeWire)
# ------------------------------------------------------------------------------
if pgrep -u "$USER" -x "pipewire" >/dev/null 2>&1; then
    pkill -9 -u "$USER" -x "pipewire|pipewire-pulse|wireplumber" 2>/dev/null
    wait_for_stop pipewire
fi

if [ -n "$XDG_RUNTIME_DIR" ]; then
    find "$XDG_RUNTIME_DIR" -maxdepth 1 \
        \( -name 'pipewire*' -o -name 'pulse*' \) \
        -exec rm -rf {} + 2>/dev/null
fi

pipewire &

# ------------------------------------------------------------------------------
# Desktop services
# ------------------------------------------------------------------------------
run picom
run dunst

# ------------------------------------------------------------------------------
# System tray applications
# ------------------------------------------------------------------------------
run nm-applet
run pasystray
run udiskie --tray
# run blueman-applet

run copyq
run flameshot

run monitor-caps-num-lock
run monitor-kbd-layout

# ------------------------------------------------------------------------------
# Applications
# ------------------------------------------------------------------------------
run Telegram -autostart
run /home/danial/.app/Throne/Throne -tray
run /home/danial/.local/bin/ABDownloadManager --background
run mailspring --background

# ------------------------------------------------------------------------------
# Status Bar
# ------------------------------------------------------------------------------
pkill -u "$USER" -x dwmblocks 2>/dev/null
wait_for_stop dwmblocks

dwmblocks &

# ------------------------------------------------------------------------------
# Local AI Proxy
# ------------------------------------------------------------------------------
# node /home/danial/.npm-global/lib/node_modules/omniroute/bin/omniroute.mjs serve --no-open --tray
# node /home/danial/.npm-global/lib/node_modules/9router/cli.js serve --no-open --tray &

# node /home/danial/.npm-global/lib/node_modules/9router/cli.js \
#   serve --no-open --tray --host 127.0.0.1 &
