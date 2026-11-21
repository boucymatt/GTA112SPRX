// PS3DEV / PSL1GHT translation unit for the original GTA V 1.12 native hooks.
// This source deliberately does not include Sony SNC / Cell SDK headers.
// It is not compiled at the same time as the legacy prx.cpp.
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/prx.h>
#include <sys/thread.h>

int G_NATIVES = 0;
int GLOBALS_PTR = 0;
int TOC = 0;
int MAIN_HOOK_ADDRESS = 0;

#include "../types.h"
#include "compat.h"
#include "../GTAStructs.h"
#include "../Natives.h"
#include "../gtaCode.h"
#include "../menu.h"

// GTA 1.12's NETWORK_IS_SIGNED_ONLINE native is the periodic callback used
// by the original mod. Preserve the native's original return instead of
// unconditionally returning "online" like the old project did.
static uint64_t originalNativeOpd = 0;
static volatile bool hookInstalled = false;
static volatile bool stopping = false;
static bool executingHook = false;
static sys_ppu_thread_t workerThread = 0;

static void restoreHook()
{
    if (!hookInstalled || MAIN_HOOK_ADDRESS < 0x10000) return;
    *reinterpret_cast<volatile uint64_t*>(MAIN_HOOK_ADDRESS) = originalNativeOpd;
    __asm__ volatile ("sync" ::: "memory");
    hookInstalled = false;
}

extern "C" void esctasy_main_hook(NativeArg_s* args)
{
    // The saved 8-byte OPD is a callable PPU function descriptor.
    typedef void (*NativeFn)(NativeArg_s*);
    NativeFn original = reinterpret_cast<NativeFn>(&originalNativeOpd);
    if (originalNativeOpd) original(args);

    if (stopping || executingHook || !G_NATIVES) return;
    executingHook = true;
    draw_menu();
    executingHook = false;
}

static bool installHook()
{
    if (hookInstalled || stopping) return hookInstalled;

    // Native lookup uses the version-specific signatures in gtaCode.h.
    int table = findNativesPointer();
    if (table < 0x10000) return false;
    G_NATIVES = table;

    int globals = findGlobalsPointer();
    if (globals >= 0x10000) GLOBALS_PTR = globals;

    int descriptor = getOPDOfNative(0x6E5BDCE2); // NETWORK_IS_SIGNED_ONLINE
    if (descriptor < 0x10000 || descriptor == -1) return false;

    uint64_t prior = *reinterpret_cast<volatile uint64_t*>(descriptor);
    uint64_t replacement = *reinterpret_cast<const uint64_t*>(
        reinterpret_cast<uintptr_t>(&esctasy_main_hook));
    if (!prior || !replacement) return false;

    MAIN_HOOK_ADDRESS = descriptor;
    TOC = *reinterpret_cast<const uint32_t*>(descriptor + 4);

    originalNativeOpd = prior;
    *reinterpret_cast<volatile uint64_t*>(descriptor) = replacement;
    __asm__ volatile ("sync" ::: "memory");
    hookInstalled = true;
    return true;
}

static void worker(void*)
{
    debug_write((char*)"Esctasy PSL1GHT module started\n");

    // Avoid scanning GTA's code while the executable is still starting.
    usleep(15000000);
    for (int i = 0; i < 45 && !stopping; ++i) {
        if (installHook()) {
            debug_append((char*)"GTA V 1.12 native hook installed\n");
            break;
        }
        usleep(1000000);
    }
    if (!hookInstalled)
        debug_append((char*)"Hook unavailable: wrong game/update/signature\n");

    sysThreadExit(0);
}

// PRX module exports; these are entrypoint definitions, not SONY SDK macros.
// WARNING: The legacy PSL1GHT toolchain does not supply a complete PRX
// metadata/signing pipeline. This source builds PPU objects, but a valid
// PRX export table and packager must be provided before loading as .sprx.
extern "C" int module_start(size_t args, void* argv)
{
    (void)args;
    (void)argv;
    stopping = false;
    int rc = sysThreadCreate(&workerThread, worker, 0, 0x5AA, 64 * 1024,
                             THREAD_JOINABLE, (char*)"EsctasyPS3");
    return rc == 0 ? SYS_PRX_RESIDENT : SYS_PRX_NO_RESIDENT;
}

extern "C" int module_stop(size_t args, void* argv)
{
    (void)args;
    (void)argv;
    stopping = true;
    if (workerThread) {
        u64 result = 0;
        sysThreadJoin(workerThread, &result);
        workerThread = 0;
    }
    restoreHook();
    return SYS_PRX_RESIDENT;
}
