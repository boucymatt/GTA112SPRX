# Generating a genuine GTA112.prx / GTA112.sprx with PS3DK

## Why the existing ELF cannot be signed directly

The old `/usr/local/ps3dev/ppu/bin/ppu-g++` (GCC 7.2.0) build **already compiles and links**, but its `build/ps3dev/GTA112.elf` is a PowerPC64 `EXEC` with retained `.rela.*` and `.opd` sections and **no PRX module-info or export records**. `scetool` can process compatible SELF containers but cannot invent a missing PRX relocation segment, module identity or startup management exports. The old `sprxlinker` cannot perform PRX authoring either.

The [PS3DK](https://github.com/FirebirdTA01/PS3DK) project has a genuine PRX authoring path: `ps3_add_prx` links against its PRX CRT and linker script, `prx-gen` emits/validates a relocatable module, and `make_sprx` / `fself` produce SPRX variants.

**Do not attempt `prx-gen build` against the already linked GCC 7.2.0 ELF. Rebuild with the PS3DK toolchain.**

## Keep the working old SDK

Install the newer PS3DK toolchain in a *different directory* so it does not overwrite your existing `/usr/local/ps3dev`. Use the [PS3DK build guide](https://github.com/FirebirdTA01/PS3DK/blob/main/docs/building.md) and [PRX guide](https://github.com/FirebirdTA01/PS3DK/blob/main/docs/tools/prx-gen.md). On Arch Linux, the guide lists:

```bash
sudo pacman -S --needed base-devel gcc gmp mpfr libmpc isl zlib readline ncurses expat openssl libelf python rust git wget bison flex texinfo cmake ninja patch zip unzip

git clone https://github.com/FirebirdTA01/PS3DK.git ~/PS3DK
cd /home/boucy/PS3DK
# IMPORTANT: a previously exported PS3DEV=/usr/local/ps3dev is retained
# by env.sh and would mix your old SDK with the new one.
unset PS3DEV PS3DK PSL1GHT PPU_PREFIX SPU_PREFIX
source ./scripts/env.sh
./scripts/bootstrap.sh
# Follow docs/building.md's ordered toolchain and SDK stages.
```

The PS3DK environment script uses `$PS3DEV` *if already defined*, otherwise it defaults to the separate `$PS3_TOOLCHAIN_ROOT/stage/ps3dev`. Unset an old `PS3DEV=/usr/local/ps3dev` before sourcing it, as shown above. `$PS3DK` then points to the staged `ps3dk` directory. Keep this shell for the modern build; a fresh terminal can keep using the old `/usr/local/ps3dev`.

Check prerequisites after installation:

```bash
sudo pacman -S --needed ninja
echo "$PS3DEV"
echo "$PS3DK"
test -f "$PS3_TOOLCHAIN_ROOT/cmake/ps3-ppu-toolchain.cmake" || echo "Missing CMake toolchain definition"
test -x "$PS3DEV/ppu/bin/powerpc64-ps3-elf-g++" || echo "Modern PPU compiler not yet built"
powerpc64-ps3-elf-g++ --version
command -v prx-gen
command -v make_sprx
test -f "$PS3DK/ppu/lib/lv2-prx-crt.o"
test -f "$PS3DK/ppu/lib/lv2-prx.ld"
test -f "$PS3DK/ppu/lib/lv2-prx.specs"
```

## Build the Esctasy module with PS3DK

The PRX configuration is now in `ps3dk/CMakeLists.txt`.

```bash
cd /home/boucy/Documents/Esctasy/GTA112SPRX
git fetch origin
git switch feature/esctasy-ps3-menu
git pull --ff-only

# Discard the partial CMake configure cache created with the old SDK.
rm -rf build/ps3dk

# Use the env.sh exports from the PS3DK source checkout.
# For Linux source installs, the documented CMake toolchain is:
cmake -S ps3dk -B build/ps3dk -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$PS3_TOOLCHAIN_ROOT/cmake/ps3-ppu-toolchain.cmake"

cmake --build build/ps3dk -v
```

If your install uses a different CMake-toolchain location, see [PS3DK toolchain documentation](https://github.com/FirebirdTA01/PS3DK/blob/main/docs/tools/toolchain.md). This recipe requires the *modern* SDK, not the older 7.2.0 compiler.

The `ps3_add_prx(... SIGN)` step requests:
- `GTA112.prx`: the relocatable module (validated with `prx-gen check`);
- `GTA112.sprx`: signing produced by `make_sprx` (**PS3DK notes this output is not yet proven to work on real hardware and may be rejected by RPCS3**);
- `GTA112.fake.sprx`: fake-signed output for the matching CFW/fake-SELF loading case.

These are **not** guaranteed runtime-tested plugins. This CMake configuration is a first attempt; compiler/runtime compatibility may need further fixes with the modern GCC 16.1.0 headers. Even a PRX that passes `prx-gen check` must be tested against the correct *GTA V 1.12 BLES* binary, since game-specific hooks are independent of SDK compatibility.

## Current status

- **GCC 7.2.0**: `make -f Makefile.ps3dev check` succeeded and `make -f Makefile.ps3dev elf` succeeded.
- **GCC 7.2.0 PRX packaging**: not supported by the provided Makefile.
- **Modern PS3DK CMake PRX pipeline**: configuration committed to this branch but **not compiled/tested** in this environment.
- **Real PS3 loading**: not tested. Make backup copies before experimenting with process hooks.

Log modern-build failures with:

```bash
cmake -S ps3dk -B build/ps3dk -G Ninja -DCMAKE_TOOLCHAIN_FILE="$PS3_TOOLCHAIN_ROOT/cmake/ps3-ppu-toolchain.cmake" 2>&1 | tee ps3dk-configure.log
cmake --build build/ps3dk -v 2>&1 | tee ps3dk-build.log
```
