/* EBOOT-side source stub for GTA V 1.12 BLES, using PS3DK headers.
 *
 * This source is intentionally NOT linked into GTA112.sprx. It must be
 * integrated into a verified, decrypted EBOOT ELF at a safe initialization
 * call site. Compiling this object alone does not patch any EBOOT.BIN.
 *
 * The EBOOT (not VSH) loads and starts our module in the GTA process.
 */
#include <stddef.h>
#include <sys/prx.h>

#define ESCTASY_MODULE_PATH "/dev_hdd0/tmp/GTA112.sprx"

static volatile int g_esctasy_loader_state = 0; /* 0=idle, 1=running, 2=loaded, -1=failed */
static int g_esctasy_module_id = -1;
static int g_esctasy_error = 0;

/* Call ONLY once the game's sysPrxForUser imports and filesystem are ready.
 * A GTA 1.12-specific EBOOT patch point still needs to be identified.
 * Returns 0 on success; <0 if it fails (or while already in progress).
 */
int esctasy_load_tmp_sprx(void)
{
    int previous = __sync_val_compare_and_swap(&g_esctasy_loader_state, 0, 1);
    if (previous != 0) return previous == 2 ? 0 : -1;

    int id = sysPrxLoadModule(ESCTASY_MODULE_PATH, 0, NULL);
    if (id < 0) {
        g_esctasy_error = id;
        g_esctasy_loader_state = -1;
        return id;
    }

    int modres = 0;
    int rc = sysPrxStartModule(id, 0, NULL, &modres, 0, NULL);
    if (rc != 0 || modres != SYS_PRX_RESIDENT) {
        g_esctasy_error = (rc != 0) ? rc : -1;
        sysPrxUnloadModule(id, 0, NULL);
        g_esctasy_loader_state = -1;
        return g_esctasy_error;
    }

    g_esctasy_module_id = id;
    g_esctasy_loader_state = 2;
    return 0;
}

/* Useful in an EBOOT debugger. */
int esctasy_loader_status(void) { return g_esctasy_loader_state; }
int esctasy_loader_last_error(void) { return g_esctasy_error; }
int esctasy_loader_module_id(void) { return g_esctasy_module_id; }
