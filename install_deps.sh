#!/bin/bash
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

log() { echo -e "${GREEN}[wyrm-install]${NC} $1"; }
err() { echo -e "${RED}[wyrm-install]${NC} $1"; exit 1; }

# ── System Dependencies ───────────────────────────────────────────────────────
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

# ── Rust ───────────────────────────────────────────────
if ! command -v cargo &> /dev/null; then
    log "Installing Rust..."
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
    source $HOME/.cargo/env
    log "Rust installed."
else
    log "Rust already installed, skipping."
fi

# ── NatNet ────────────────────────────────────────────────────────────────────
log "Installing NatNet SDK..."

NATNET_ARCHIVE=$(find $HOME/Downloads -name "*.tar" | grep -i natnet | head -1)
if [ -z "$NATNET_ARCHIVE" ]; then
    err "Could not find NatNet archive in ~/Downloads. Please download it from https://optitrack.com and place it in ~/Downloads."
fi

log "Found NatNet archive: $NATNET_ARCHIVE"
mkdir -p /tmp/NatNetSDK
tar -xf "$NATNET_ARCHIVE" -C /tmp/NatNetSDK
mkdir -p $HOME/.local/include/NatNet
cp /tmp/NatNetSDK/include/* $HOME/.local/include/NatNet/
cp /tmp/NatNetSDK/lib/libNatNet.so $HOME/.local/lib/
rm -rf /tmp/NatNetSDK
log "NatNet SDK installed."

# ── Zenoh-C ───────────────────────────────────────────────────────────────────
log "Installing zenoh-c..."

git clone https://github.com/eclipse-zenoh/zenoh-c.git /tmp/zenoh-c
cd /tmp/zenoh-c && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --build . -j$(nproc)
cmake --install .
cd / && rm -rf /tmp/zenoh-c
log "zenoh-c installed."

# ── Zenoh-CPP ─────────────────────────────────────────────────────────────────
log "Installing zenoh-cpp..."

git clone https://github.com/eclipse-zenoh/zenoh-cpp.git /tmp/zenoh-cpp
cd /tmp/zenoh-cpp && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local -DCMAKE_PREFIX_PATH=$HOME/.local
cmake --build . -j$(nproc)
cmake --install .
cd / && rm -rf /tmp/zenoh-cpp
log "zenoh-cpp installed."

# ── Fast-CDR ─────────────────────────────────────────────────────────────────
log "Installing Fast-CDR..."

git clone https://github.com/eProsima/Fast-CDR.git /tmp/Fast-CDR
cd /tmp/Fast-CDR && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
cmake --build . --target install
cd / && rm -rf /tmp/Fast-CDR
log "Fast-CDR installed."

# ── Environment ───────────────────────────────────────────────────────────────
if ! grep -q "CMAKE_PREFIX_PATH" $HOME/.bashrc; then
    echo 'export CMAKE_PREFIX_PATH=$HOME/.local:$CMAKE_PREFIX_PATH' >> $HOME/.bashrc
    log "Added CMAKE_PREFIX_PATH to $HOME/.bashrc"
else
    log "CMAKE_PREFIX_PATH already in $HOME/.bashrc, skipping."
fi

log "All dependencies installed. Run 'source ~/.bashrc' to update your environment."