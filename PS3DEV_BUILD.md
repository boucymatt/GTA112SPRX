# Building Esctasy for PS3DEV / PSL1GHT

This is a **PS3DEV source port** of the GTA V 1.12 BLES menu's low-level host layer. It is based on the [PS3DEV/PSL1GHT SDK](https://github.com/ps3dev/PSL1GHT), *not* the old Sony SNC Visual Studio PS3 SDK. It retains the Esctasy `menu.h`, native invocation wrappers and GTA-specific signature scans.

## Environment (Arch Linux)

Use your existing PS3DEV installation (typically `/usr/local/ps3dev`).

```bash
export PS3DEV=/usr/local/ps3dev
export PSL1GHT="$PS3DEV"
# For installations that keep PSL1GHT under ps3dev/psl1ght, use:
# export PSL1GHT="$PS3DEV/psl1ght"
export PATH="$PS3DEV/bin:$PS3DEV/ppu/bin:$PATH"

ppu-g++ --version
test -f "$PSL1GHT/ppu/include/sys/thread.h" && echo "PSL1GHT headers found"
```

## Cross-compile

```bash
git clone https://github.com/boucymatt/GTA112SPRX.git
cd GTA112SPRX
git switch feature/esctasy-ps3-menu

make -f Makefile.ps3dev check
# Object output: build/ps3dev/prx_ps3dev.o

# Optional: attempt a normal PPU ELF link (not an SPRX):
make -f Makefile.ps3dev elf

make -f Makefile.ps3dev clean
```

The default `make -f Makefile.ps3dev` target compiles the source into a PPU object. It uses `GTA112SPRX/ps3dev/prx_ps3dev.cpp`, not the old `GTA112SPRX/prx.cpp`.

**Do not copy `build/ps3dev/GTA112.elf` to your PS3 as `GTA112.sprx`.** An ELF is not a loadable plugin.

## SDK changes made

- `#include <sys/thread.h>` and `sysThreadCreate` / `sysThreadJoin` / `sysThreadExit` replace the Sony-only PPU thread API.
- Uses `<sys/prx.h>` only for the PRX resident result constants. The new `module_start` and `module_stop` symbols are present.
- The logging / timer / memory-patching helpers live in `GTA112SPRX/ps3dev/compat.h` using standard C calls and a PPU instruction-cache flush.
- The original native hook's result is preserved instead of forcing the original native to return `true`.
- Native arguments are kept in module-owned memory under `ESCTASY_PS3DEV`. This removes two risky hard-coded writable addresses.
- The original `prx.cpp` and SNC Visual Studio project remain intact as historical reference.

## Still needed for a *loadable* SPRX

The classic PSL1GHT SDK is primarily a homebrew application build system. Its `sprxlinker` is used to process executable imports and to construct external-library **stub libraries**. It does not by itself turn an arbitrary ELF into a finished GTA plugin.

A valid PRX export/management record, module metadata, position relocations, and a matching SPRX packaging/signing stage are needed. This repository does **not** yet provide or claim a working SPRX packaging implementation for the classic PS3DEV toolchain. The `make -f Makefile.ps3dev sprx` target intentionally stops with an explanatory error.

Even once a compatible PRX packager is available, the target hooks are **GTA V 1.12 BLES-specific**. Test only on an appropriate CFW PS3. Keep a backup of any existing working plugin. Loading into a different game/update can crash the game. Memory hooks need permission to patch process memory; ordinary homebrew permissions may not be sufficient.

## Debugging

The port logs early startup information to `/dev_hdd0/tmp/esctasy_ps3dev.log` if the SDK's C filesystem API is available in the loaded runtime.

If the source fails to compile, run:

```bash
make -f Makefile.ps3dev check VERBOSE=1 2>&1 | tee ps3dev-build.log
```

Use the exact errors in `ps3dev-build.log` to finish compatibility adjustments for the installed version of the SDK.

**Status:** Written against the public PSL1GHT header interface and the project's existing headers, but not compiled with `ppu-g++` or tested on a console in this environment.
