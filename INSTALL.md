# Install, Compile, and Run

---

## Dependencies

`cmake`

`make`

`g++`

`libfreetype6-dev`

SDL2 graphics library
`libsdl2-dev`

SDL2 TTF for GUI
`libsdl2-ttf-dev`

SDL2 Image for GUI
`libsdl2-image-dev`

---

## Compilation

```
sh .configure.sh
```
or
```
mkdir build
cd build
cmake ..
make
```

### Backend selection (SDL2 and/or OpenGL)

The library can be built with either backend independently, or with both. You can control this in two ways:

1) Via the helper script:
```
# Both backends (default)
sh .configure.sh

# SDL2 only
sh .configure.sh sdl
# or explicit toggles
sh .configure.sh sdl=on opengl=off

# OpenGL only
sh .configure.sh opengl
# or explicit toggles
sh .configure.sh sdl=off opengl=on

# Disable both (not useful for running the GUI, but supported)
sh .configure.sh sdl=off opengl=off
```

2) Via CMake options directly:
```
mkdir build && cd build
cmake -DENABLE_SDL2=ON -DENABLE_OPENGL=OFF ..   # SDL2 only
cmake -DENABLE_SDL2=OFF -DENABLE_OPENGL=ON ..   # OpenGL only
cmake -DENABLE_SDL2=ON -DENABLE_OPENGL=ON  ..   # Both (default)
make -j$(nproc)
```

Notes:
- When SDL2 is disabled, GUI-related targets (`Frontend/GUI`, `Frontend/GItems`, `Frontend/RUGraph`, `Frontend/GLayouts`) are not built.
- When OpenGL is enabled, the OpenGL/GLFW text path is available.

---

## Installation

make install

---

## Uninstall

make uninstall
