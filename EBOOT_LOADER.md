# GTA V 1.12 BLES: EBOOT loader and /dev_hdd0/tmp placement

## Workflow

The menu is a PRX loaded **in the GTA V process**, not a VSH plugin loaded
when the PS3 reaches the XMB. Producing `GTA112.sprx` by itself does **not**
cause GTA to load it.

1. Build the SPRX with PS3DK and verify `prx-gen check` reports `OK`.
2. Patch the **matching GTA V 1.12 BLES EBOOT.BIN** (or its decrypted ELF
   representation) to load and start the module *after game initialization*.
3. Install the modified EBOOT into the matching game's `USRDIR` location.
   On the normal BLES01807 installed-update layout this is
   `/dev_hdd0/game/BLES01807/USRDIR/EBOOT.BIN`; verify the actual Title ID
   and which EBOOT is executed before replacing any file.
4. Copy the appropriately signed/fake-signed build to
   **`/dev_hdd0/tmp/GTA112.sprx`**, matching the string in the EBOOT loader.
5. Start the matching game on CFW and test only after backups have been made.

Do **not** put this game module into `boot_plugins.txt`; that would load it
in the system shell rather than inside GTA V, where its hardcoded game
addresses and native table are valid.

## What the repository now supplies

`GTA112SPRX/eboot_loader/loader.c` is PS3DEV/PS3DK-compatible **source code**
for the EBOOT side. Its `esctasy_load_tmp_sprx()` calls

```c
sysPrxLoadModule("/dev_hdd0/tmp/GTA112.sprx", 0, NULL);
sysPrxStartModule(module_id, 0, NULL, &module_result, 0, NULL);
```

with errors checked, and calls once only. The CMake project compiles it as
a separate `EsctasyEbootLoader` **object**; it is deliberately not linked
into the SPRX, since recursively loading itself would accomplish nothing.

An actual EBOOT patch still needs a reliable, version-specific branch/call
site and space for the loader code. That patch cannot safely be chosen from
the SPRX build alone. This repository currently does not contain a game
EBOOT or a verified patch offset; no modified EBOOT has been produced.

## What is needed to make the EBOOT patch

Provide a backup copy of the exact original `EBOOT.BIN` that runs with your
GTA V 1.12 BLES installation (preferably also the decrypted ELF if you have
one) and the Title ID. If extracting from an update, use the version-specific
EBOOT currently being launched, not just the file on the retail disc.

On your PC, note the checksums before making any changes:

```bash
sha256sum EBOOT.BIN
file EBOOT.BIN
scetool --info EBOOT.BIN
```

Once its ELF segments and layout are known, a patch can be designed that
keeps the original initialization behavior, adds/imports the PRX load/start
calls, places the module path, and redirects one verified game startup call
site. Then the patched ELF must be repackaged as an appropriate EBOOT.BIN
for the specific CFW CEX/DEX environment, not just renamed.

Back up the stock EBOOT before changing it. An incorrect EBOOT patch usually
prevents the game from booting and can require restoring the original file.
The PRX signing output from this new PS3DK toolchain is not yet confirmed to
be compatible with your real PS3 runtime; a `prx-gen check OK` is structural,
not a proof of runtime loadability or GTA hook safety.

## EBOOT-side build

The `ps3dk/CMakeLists.txt` now compiles two **separate** targets:
- `Esctasy112` -> `GTA112.prx`, `GTA112.sprx`, `GTA112.fake.sprx`
- `EsctasyEbootLoader` -> a PowerPC object for incorporation into a
  decrypted EBOOT; not a complete EBOOT.BIN.

Running `cmake --build build/ps3dk -v` builds both. The existing SPRX
artifacts remain independent of the EBOOT-loader object.
