# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What is this project?

DOSBox-X is a cross-platform DOS emulator forked from DOSBox. Beyond DOS gaming, it targets accurate emulation of DOS/Windows 3.x/9x/ME, NEC PC-98, and DOS/V systems. It supports both SDL1 and SDL2.

## Build Commands

### Windows (CMake + Ninja)
```
# Generate (one-time, from repo root):
cmake -B build/ninja -G Ninja

# Build:
ninja -C build/ninja
```
Output: `build/ninja/dosbox-x.exe` (debug, ~38MB)

### Windows (Visual Studio)
Open `vs/dosbox-x.sln`. Select Win32/x64 and SDL1/SDL2 configuration. Build.

### Linux / macOS (autotools)
```
./autogen.sh
./configure
make -j$(nproc)
```

There is no test suite.

## CMake Build Architecture (Windows)

The CMake build (`CMakeLists.txt`) compiles six bundled third-party libraries from source as static libraries, since only headers ship in `vs/` — no pre-built x64 `.lib` files exist:

| Target | Source location | Key notes |
|---|---|---|
| `sdl2_bundled` | `vs/sdl2/src/` (~184 files) | Windows-only subset; also adds `SDL_windows_main.c` for WinMain |
| `sdl2net_bundled` | `vs/sdl2net/` (4 files) | |
| `zlib_bundled` | `vs/zlib/` (15 files) | |
| `libpng_bundled` | `vs/libpng/` (15 files) | Depends on zlib |
| `freetype_bundled` | `vs/freetype/src/` (46 files) | Needs `FT2_BUILD_LIBRARY` define |
| `pdcurses_bundled` | `vs/libpdcurses/` (49 files) | Only built when `ENABLE_DEBUG=ON` |

**Known quirks:**
- `vs/pcap` include directory must appear **before** `vs/` in the include path order, or `vs/pcap/pcap.h` (a guard-less compat shim that does `#include <pcap/pcap.h>`) will infinitely recurse through the `vs/` include path instead of finding `vs/pcap/pcap/pcap.h`.
- The project root must be in the include path because `include/mapper.h` uses `#include "include/menu.h"` (path relative to repo root, not to the `include/` directory).
- `_nhandle` (in `src/dos/dos_network2.h`) is a legacy CRT symbol not exported by UCRT (VS2015+). It's defined as a constant for `_MSC_VER >= 1900`.

## Source Architecture

**Entry point:** `src/gui/sdlmain.cpp` — `main()` (rewritten to `SDL_main` on Windows, with `WinMain` provided by SDL2). Contains emulator setup, runtime loop, GFX management, and menu handling.

**Configuration system:** `src/dosbox.cpp` defines sections and settings via `DOSBox_SetupConfigSections()`. Settings are accessed globally through `control` pointer. Each setting has a type (int, hex, string, double, multivalue). See `include/setup.h` and `src/misc/setup.cpp`.

**Key subsystems:**

- `src/cpu/` — CPU emulation with multiple core implementations: `core_normal` (default), `core_simple`, `core_prefetch`, `core_full`, `core_dyn_x86` (dynamic recompiler for x86), `core_dynrec` (portable dynamic recompiler)
- `src/hardware/` — Hardware emulation: VGA (`vga_*.cpp`), Sound Blaster (`sblaster.cpp`), OPL/Adlib (`adlib.cpp`, `opl.cpp`), DMA, PIC, PCI, keyboard, floppy, IDE, 3Dfx Voodoo, NE2000 networking, PC-98 FM sound (`snd_pc98/`)
- `src/dos/` — DOS kernel: file operations, memory management, MSCDEX, keyboard layouts, drive implementations (local, FAT, ISO, overlay, virtual, physfs)
- `src/ints/` — BIOS and interrupt handlers: INT 10h (video), INT 13h (disk), EMS, XMS, mouse
- `src/gui/` — SDL interface, mapper (keyboard/joystick bindings), menu system, MIDI, rendering and scalers
- `src/debug/` — Internal debugger (requires curses/pdcurses), disassembler
- `src/output/` — Display output backends: surface, OpenGL, Direct3D 9/11, TTF
- `src/shell/` — DOS command shell (COMMAND.COM emulation)
- `src/builtin/` — Built-in DOS executables as embedded byte arrays

**Bundled libraries** (in `vs/` for Windows, or system libraries on Linux/macOS):
- SDL2 (modified in-tree version with IME and threading improvements)
- SDL2_net, zlib, libpng, FreeType, pdcurses
- MT-32 emulator (`src/libs/mt32/`), FluidSynth (`src/libs/fluidsynth/`), xBRZ scaler (`src/libs/xBRZ/`)

## Integer Type Conventions

Never assume `int` or `long` sizes. Use `uintptr_t` for pointer arithmetic. On Windows x64, `long` is 32-bit (unlike Linux where it's 64-bit). The codebase uses DOSBox-era typedefs `Bitu`/`Bits` (unsigned/signed machine-word-width integers) alongside C99 fixed-width types.
