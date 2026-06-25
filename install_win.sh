#!/bin/bash
set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'
log() { echo -e "${GREEN}[wyrm-install]${NC} $1"; }
err() { echo -e "${RED}[wyrm-install]${NC} $1"; exit 1; }

# WSL detection (informational)
if grep -qi microsoft /proc/version 2>/dev/null; then
    log "WSL detected — NatNet multicast may require 'mirrored' networking mode in .wslconfig"
fi

# System Dependencies
log "Installing system dependencies..."
if [ -f /etc/debian_version ]; then
    sudo apt update
    sudo apt install -y build-essential make cmake tar curl zip unzip git
elif [ -f /etc/fedora-release ]; then
    sudo dnf update -y
    sudo dnf install -y gcc gcc-c++ make cmake tar curl zip unzip git
else
    err "Unsupported OS. Please install build tools manually and re-run this script."
fi
log "System dependencies installed."

# Rust
if ! command -v cargo &> /dev/null; then
    log "Installing Rust..."
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
    source "$HOME/.cargo/env"
    # Persist for future shells
    if ! grep -q 'cargo/env' "$HOME/.bashrc"; then
        echo 'source "$HOME/.cargo/env"' >> "$HOME/.bashrc"
    fi
    log "Rust installed."
else
    log "Rust already installed, skipping."
fi

# NatNet
log "Installing NatNet SDK..."
NATNET_ARCHIVE=$(find "$HOME/Downloads" -name "*.tar" | grep -i natnet | head -1)
if [ -z "$NATNET_ARCHIVE" ]; then
    err "Could not find NatNet archive in ~/Downloads. Download from https://optitrack.com and place it there."
fi
log "Found NatNet archive: $NATNET_ARCHIVE"
mkdir -p /tmp/NatNetSDK
tar -xf "$NATNET_ARCHIVE" -C /tmp/NatNetSDK

# Handle potential subdirectory in archive
NATNET_ROOT=$(find /tmp/NatNetSDK -maxdepth 2 -name "include" -type d | head -1 | xargs dirname)
[ -z "$NATNET_ROOT" ] && NATNET_ROOT=/tmp/NatNetSDK

mkdir -p "$HOME/.local/include/NatNet" "$HOME/.local/lib"
cp "$NATNET_ROOT"/include/* "$HOME/.local/include/NatNet/"
cp "$NATNET_ROOT"/lib/libNatNet.so "$HOME/.local/lib/"
rm -rf /tmp/NatNetSDK
log "NatNet SDK installed."

# Cyclone DDS
log "Installing Cyclone-DDS..."
git clone https://github.com/eclipse-cyclonedds/cyclonedds.git /tmp/cyclonedds
cd /tmp/cyclonedds && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX="$HOME/.local"
cmake --build . -j"$(nproc)"
cmake --install .
cd / && rm -rf /tmp/cyclonedds
log "Cyclone-DDS installed."

# Environment variables
EXPORTS=(
    'export CMAKE_PREFIX_PATH=$HOME/.local:$CMAKE_PREFIX_PATH'
    'export LD_LIBRARY_PATH=$HOME/.local/lib:$LD_LIBRARY_PATH'   # fixes libNatNet.so
    'export PKG_CONFIG_PATH=$HOME/.local/lib/pkgconfig:$PKG_CONFIG_PATH'
)

for line in "${EXPORTS[@]}"; do
    key=$(echo "$line" | grep -o 'export [^=]*' | awk '{print $2}')
    if ! grep -q "$key" "$HOME/.bashrc"; then
        echo "$line" >> "$HOME/.bashrc"
        log "Added $key to ~/.bashrc"
    else
        log "$key already in ~/.bashrc, skipping."
    fi
done

log "All dependencies installed. Run 'source ~/.bashrc' to update your environment."

# WSL networking reminder
if grep -qi microsoft /proc/version 2>/dev/null; then
    echo ""
    log "WSL reminder: if NatNet can't find Motive, add this to %USERPROFILE%/.wslconfig on Windows:"
    log "  [wsl2]"
    log "  networkingMode=mirrored"
fi