# Install, Compile, and Run

---

## Dependencies

### Debian

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

`sudo apt-get install libglfw3 libglfw3-dev`

### Fedora

sudo dnf install -y gcc gcc-c++ clang cmake make
sudo dnf install -y freetype-devel
sudo dnf install -y libasan

sudo dnf install -y mesa-libGL-devel mesa-libGLU-devel libglvnd-devel
sudo dnf install -y glfw-devel

### Windows

See the [Windows](#windows-build) section below.

---

## Linux Build

### Compilation (prod)

Requires shmea to be installed (`make install` from ShmeaDB).

```
mkdir build
cd build
cmake ..
make
```

Or using presets:
```
cmake --preset linux-release
cmake --build --preset linux-release
```

### Compilation (dev)

Dev mode copies shmea headers from the ShmeaDB source tree into `include/` so they stay in sync. Shmea must still be installed for linking.

```
mkdir build
cd build
cmake .. -DDEV_MODE=ON
make
```

By default, `SHMEA_SOURCE_DIR` points to `../ShmeaDB`. Override it if your ShmeaDB source is elsewhere:
```
cmake .. -DDEV_MODE=ON -DSHMEA_SOURCE_DIR=/path/to/ShmeaDB
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

### Installation

```
make install
```

### Uninstall

```
make uninstall
```

---

## Windows Build

### Prerequisites

1. **Visual Studio 2022 Build Tools** with the "Desktop development with C++" workload
2. **vcpkg** with `VCPKG_ROOT` environment variable set:
   ```powershell
   vcpkg install freetype:x64-windows
   ```
3. **Ninja** (included with VS Build Tools or install separately)
4. **ShmeaDB** built and installed:
   ```powershell
   cd ShmeaDB
   .\build-and-install.bat
   ```

### Prod build

Uses shmea headers from the installed ShmeaDB. Default backend is OpenGL.

```powershell
.\build-and-install.bat
```

With SDL2 backend:
```powershell
.\build-and-install.bat 2022 sdl2
```

### Dev build

Copies shmea headers from the ShmeaDB source tree (expected at `..\ShmeaDB`) into `include/`. Shmea must still be installed for linking.

```powershell
.\dev-build.bat
```

### Dev vs Prod

| | Prod (`build-and-install.bat`) | Dev (`dev-build.bat`) |
|---|---|---|
| Shmea headers | From installed shmea | Copied from ShmeaDB source tree |
| Shmea library | Installed (`shmea.dll`) | Installed (`shmea.dll`) |
| `include/` folder | Not created | Created with fresh headers |
| Use case | CI, releases, end users | Active development |

The `include/` directory is gitignored and regenerated on each dev configure.
