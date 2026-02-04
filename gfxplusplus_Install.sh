#!/bin/bash

# Configuration
BUILD_DIR="build"
INSTALL_PREFIX="$HOME/.local"

# Create build directory
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Run CMake
cmake .. \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
    -DENABLE_SDL2=OFF \
    -DENABLE_OPENGL=ON

# Build and Install
make -j$(nproc)
make install

echo "gfxplusplus installation complete. Installed to $INSTALL_PREFIX"
