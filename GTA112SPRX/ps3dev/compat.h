// PS3DEV / PSL1GHT compatibility functions for the legacy GTA V hooks.
// Included after the standard library headers and types.h, BEFORE GTAStructs.h.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/thread.h>

#define __ALWAYS_INLINE static inline __attribute__((always_inline))
#define __NO_INLINE __attribute__((noinline))
#define __NAKED __attribute__((naked)) __attribute__((noinline))

// The target game's PPU ABI uses 8-byte function descriptors (entry, TOC).
// We keep its original descriptor for chaining native hooks.
template<typename T>
__ALWAYS_INLINE T (*call(unsigned int address, unsigned int toc = TOC))(...)
{
    volatile opd_s descriptor = {static_cast<int32_t>(address), static_cast<int32_t>(toc)};
    return reinterpret_cast<T (*)(...)>(const_cast<opd_s*>(&descriptor));
}

// On CFW where the target page is writable, change 4 PPU instructions.
// A function pointer points to an OPD, not its first instruction.
static bool PatchInJump(uintptr_t target, uintptr_t detour, bool linked)
{
    if (target < 0x10000 || detour < 0x10000) return false;
    uint32_t entry = *reinterpret_cast<const uint32_t*>(detour);
    if (entry < 0x10000) return false;
    uint32_t upper = (entry + 0x8000U) >> 16;
    uint32_t patch[4] = {
        0x3D600000U | (upper & 0xFFFFU),  // lis r11, high adjusted
        0x396B0000U | (entry & 0xFFFFU), // addi r11, r11, low
        0x7D6903A6U,                   // mtctr r11
        linked ? 0x4E800421U : 0x4E800420U
    };
    volatile uint32_t* dst = reinterpret_cast<volatile uint32_t*>(target);
    for (int i = 0; i < 4; ++i) dst[i] = patch[i];
    __asm__ volatile ("sync" ::: "memory");
    __builtin___clear_cache(reinterpret_cast<char*>(target),
                            reinterpret_cast<char*>(target + sizeof(patch)));
    __asm__ volatile ("isync" ::: "memory");
    return true;
}

static void createFile(char* path)
{
    FILE* file = fopen(path, "wb");
    if (file) fclose(file);
}
static void writeToFile(char* path, char* contents = 0, int length = 0)
{
    FILE* file = fopen(path, "wb");
    if (!file) return;
    if (contents && length > 0) fwrite(contents, 1, static_cast<size_t>(length), file);
    fclose(file);
}
static void appendToFile(char* path, char* contents = 0, int length = 0)
{
    FILE* file = fopen(path, "ab");
    if (!file) return;
    if (contents && length > 0) fwrite(contents, 1, static_cast<size_t>(length), file);
    fclose(file);
}
static void debug_write(char* msg)
{
    if (msg) writeToFile((char*)"/dev_hdd0/tmp/esctasy_ps3dev.log", msg, strlen(msg));
}
static void debug_append(char* msg)
{
    if (msg) appendToFile((char*)"/dev_hdd0/tmp/esctasy_ps3dev.log", msg, strlen(msg));
}

// Required by gtaCode.h's optional controller diagnostics.
static char* itoa(unsigned int number, char* out, int base)
{
    if (!out) return out;
    if (base == 10) snprintf(out, 16, "%u", number);
    else if (base == 16) snprintf(out, 16, "%x", number);
    else {
        int i = 0;
        do { unsigned digit = number % (unsigned)base;
             out[i++] = static_cast<char>(digit < 10 ? '0' + digit : 'a' + digit - 10);
             number /= (unsigned)base;
        } while (number && i < 14);
        out[i] = '\0';
        for (int j = 0; j < i/2; ++j) {
            char ch = out[j]; out[j] = out[i-j-1]; out[i-j-1] = ch;
        }
    }
    return out;
}
static char* ItoS(int number)
{
    static char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d", number);
    return buffer;
}
static char* ItoSH(int number) { return ItoS(number); }

// The old source treats sleep()'s argument as milliseconds.
static void sleep(unsigned int ms)
{
    usleep(static_cast<useconds_t>(ms) * 1000U);
}
