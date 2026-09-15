# Personal dwm Build

A customized X11 desktop environment centered around `dwm-flexipatch`, bundled with `dwmblocks-async`, `dmenu-flexipatch`, and custom automation scripts for notifications, fan profiles, and hardware management.

---

## Overview

This repository houses my modular Suckless desktop setup. Built using the flexipatch pre-processor structure, it allows fast toggling and customization of patches while maintaining clean, maintainable configuration files.

* **Window Manager**: `dwm-flexipatch` (Dynamic Window Manager with patch management)

* **Status Bar**: `dwmblocks-async` (Fast, asynchronous modular status monitor)

* **Application Launcher**: `dmenu-flexipatch`

* **Configuration & Scripts**: Located under `my-configs/` (hardware management, notification handlers, keyboard switching, fan platform profiles)

---

## Included Submodules

* `dwm-flexipatch`

* `dmenu-flexipatch`

* `dwmblocks-async`

* `flexipatch-finalizer`

---

## Prerequisites

On Debian/Ubuntu-based systems:

```bash
sudo apt install build-essential libx11-dev libxft-dev libxinerama-dev libimlib2-dev

```

On Void Linux:

```bash
sudo xbps-install -S base-devel libX11-devel libXft-devel libXinerama-devel imlib2-devel

```

On Arch Linux:

```bash
sudo pacman -S base-devel libx11 libxft libxinerama imlib2

```

---

## Installation & Setup

1. **Clone the repository with submodules**:

```bash
git clone --recurse-submodules https://github.com/danialmd81/dwm.git
cd dwm

```

2. **Build and install components**:

* **dwm**:

```bash
cd dwm-flexipatch
sudo make clean install
cd ..

```

* **dwmblocks**:

```bash
cd dwmblocks-async
sudo make clean install
cd ..

```

* **dmenu**:

```bash
cd dmenu-flexipatch
sudo make clean install
cd ..

```

3. **Link Configurations & Scripts**:
Ensure executable scripts inside `my-configs/` are available in your `$PATH` (e.g., `~/.local/bin` or `/usr/local/bin`) and status bar scripts are properly configured for `dwmblocks`.

---

## Running dwm

Add the following to your `~/.xinitrc` or run it from your session start script:

```bash
# Start dwmblocks in the background
dwmblocks &

# Start custom daemons or scripts from my-configs here

# Exec dwm
exec dwm

```
