
#include <Common.h>
#include <stdint.h>
#include "plugin_common.h"

/*
 * Diagnostico passivo baseado no hook usado pelo plugin oficial
 * FrameTime Logger do GoldHEN.
 */

int32_t sceGnmSubmitAndFlipCommandBuffers(
    uint32_t count,
    void *dcbGpuAddrs[],
    uint32_t *dcbSizesInBytes,
    void *ccbGpuAddrs[],
    uint32_t *ccbSizesInBytes,
    uint32_t videoOutHandle,
    uint32_t displayBufferIndex,
    uint32_t flipMode,
    int64_t flipArg
);

HOOK_INIT(sceGnmSubmitAndFlipCommandBuffers);

static volatile int g_probe_flip_logged = 0;
static int g_probe_initialized = 0;

int32_t sceGnmSubmitAndFlipCommandBuffers_hook(
    uint32_t count,
    void *dcbGpuAddrs[],
    uint32_t *dcbSizesInBytes,
    void *ccbGpuAddrs[],
    uint32_t *ccbSizesInBytes,
    uint32_t videoOutHandle,
    uint32_t displayBufferIndex,
    uint32_t flipMode,
    int64_t flipArg
)
{
    int32_t result = HOOK_CONTINUE(
        sceGnmSubmitAndFlipCommandBuffers,
        int32_t (*)(uint32_t, void **, uint32_t *, void **, uint32_t *,
                    uint32_t, uint32_t, uint32_t, int64_t),
        count,
        dcbGpuAddrs,
        dcbSizesInBytes,
        ccbGpuAddrs,
        ccbSizesInBytes,
        videoOutHandle,
        displayBufferIndex,
        flipMode,
        flipArg
    );

    if (!g_probe_flip_logged++)
    {
        final_printf(
            "[DGProbe] GNM flip count=%u videoOut=%u buffer=%u mode=%u arg=%lld result=%d\n",
            count,
            videoOutHandle,
            displayBufferIndex,
            flipMode,
            (long long)flipArg,
            result
        );
    }

    return result;
}

void FrameProbe_Init(void)
{
    if (g_probe_initialized)
        return;

    g_probe_flip_logged = 0;
    HOOK32(sceGnmSubmitAndFlipCommandBuffers);
    g_probe_initialized = 1;

    final_printf("[DGProbe] GNM frame probe initialized\n");
}

void FrameProbe_Shutdown(void)
{
    if (!g_probe_initialized)
        return;

    UNHOOK(sceGnmSubmitAndFlipCommandBuffers);
    g_probe_initialized = 0;

    final_printf("[DGProbe] GNM frame probe stopped\n");
}
