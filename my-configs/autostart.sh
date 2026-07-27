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
# Update D-Bus activation environment for X11 session variables
if command -v dbus-update-activation-environment >/dev/null 2>&1; then
    dbus-update-activation-environment DISPLAY XAUTHORITY SSH_AUTH_SOCK XDG_RUNTIME_DIR
fi

# Start GNOME Keyring daemon and export SSH authentication socket
if command -v gnome-keyring-daemon >/dev/null 2>&1; then
    eval $(gnome-keyring-daemon --start --components=secrets,ssh)
    export SSH_AUTH_SOCK
fi

# Launch Polkit authentication agent for privilege escalation prompts
if [ -f /usr/libexec/polkit-gnome-authentication-agent-1 ]; then
    run /usr/libexec/polkit-gnome-authentication-agent-1
elif [ -f /usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1 ]; then
    run /usr/lib/polkit-gnome/polkit-gnome-authentication-agent-1
fi

# ------------------------------------------------------------------------------
# Input & Display Settings
# ------------------------------------------------------------------------------
# Configure dual keyboard layout toggling (US / Persian)
setxkbmap -layout "us,ir"

# Enable Xorg screensaver and DPMS timers (10 minutes / 600 seconds)
xset s 600 600
xset dpms 600 600 600

# ------------------------------------------------------------------------------
# Screen Locking & Idle Management
# ------------------------------------------------------------------------------
# Listen for X screensaver / system suspend events and launch slock
run xss-lock --transfer-sleep-lock -- slock

# Manage idle timeout, screen dimming, and lock trigger:
# - Dims screen to 20% at 9.5 mins (-notify 30s before 10m lock)
# - Restores brightness if activity is detected before locking
# - Activates xss-lock to handle slock cleanly at 10 mins
run xautolock \
  -time 10 \
  -notify 30 \
  -notifier "brightnessctl -s set 20%" \
  -reset "brightnessctl -r" \
  -locker "xset s activate"

# ------------------------------------------------------------------------------
# System Daemons & Services
# ------------------------------------------------------------------------------
run pipewire
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
run /home/danial/.local/ABDownloadManager/bin/ABDownloadManager --background
run mailspring --password-store="gnome-libsecret" --background
run Telegram -autostart
run /home/danial/.app/Throne/Throne -tray

# ------------------------------------------------------------------------------
# Status Bar
# ------------------------------------------------------------------------------
# Terminate existing dwmblocks instances before launching a new session
pkill -9 -x dwmblocks 2>/dev/null
while pgrep -u $UID -x dwmblocks >/dev/null; do sleep 0.1; done

# Launch dwmblocks status bar
dwmblocks &