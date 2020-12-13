# Install, Compile, and Run

---

## Dependencies

Linux/Cygwin:

`cmake`

`make`

`gcc`

`g++`

libgl1-mesa-dev

`libGL-devel`

`libGL1`

`xorg-server`

`SDL2 graphics library: libsdl2-dev`

`SDL2 TTF for GUI: libsdl2-ttf-dev`

`SDL2 Image for GUI: libsdl2-image-dev`

`OpenSSL: libssl-dev`

Cygwin:

`xinit`

MacOS:

Install homebrew, a macOS package manager.

`/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`
 
`brew install cmake`

Install Xcode command line tools.

`$ xcode-select --install`

`$ brew install pkgconfig`

`brew install openssl`

`brew link --force openssl` (Follow instructions to set environment paths)

`brew install sdl2`

`brew install sdl_ttf`

---

## Installation

sudo make install

---

## Compilation

Linux/MacOS/Windows/Cygwin:
```
mkdir build
cd build
cmake ..
make
```
