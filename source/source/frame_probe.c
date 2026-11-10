
#include <Common.h>
#include <stdint.h>
#include "plugin_common.h"

typedef struct
{
    uint32_t size;
    uint32_t format;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t aspectRatio;
    uint32_t tilingMode;
    uint32_t tileMode;
    uint32_t numBuffers;
} DaysGoneVideoOutAttr;

static volatile int g_probe_gnm_flip = 0;
static volatile int g_probe_gnm_submit = 0;
static volatile int g_probe_video_flip = 0;
static volatile int g_probe_video_register = 0;

extern int sceGnmSubmitAndFlipCommandBuffers(
    void *dcb,
    uint32_t *dcbSize,
    void *ccb,
    uint32_t *ccbSize,
    int handle,
    int bufferIndex,
    int flipMode,
    int flipArg
);

extern int sceGnmSubmitCommandBuffers(
    uint32_t count,
    void **dcb,
    uint32_t *dcbSize,
    void **ccb,
    uint32_t *ccbSize
);

extern int sceVideoOutSubmitFlip(
    int handle,
    int bufferIndex,
    int flipMode,
    int flipArg
);

extern int sceVideoOutRegisterBuffers(
    int handle,
    int startIndex,
    void *buffers,
    int bufferNum,
    DaysGoneVideoOutAttr *attr
);

static int probe_gnm_flip(
    void *dcb,
    uint32_t *dcbSize,
    void *ccb,
    uint32_t *ccbSize,
    int handle,
    int bufferIndex,
    int flipMode,
    int flipArg
)
{
    int result = HOOK_CONTINUE(
        sceGnmSubmitAndFlipCommandBuffers,
        dcb, dcbSize, ccb, ccbSize,
        handle, bufferIndex, flipMode, flipArg
    );

    if (!g_probe_gnm_flip++)
    {
        final_printf(
            "[DGProbe] GNM flip handle=%d buffer=%d mode=%d arg=%d result=%d\n",
            handle, bufferIndex, flipMode, flipArg, result
        );
    }

    return result;
}

static int probe_gnm_submit(
    uint32_t count,
    void **dcb,
    uint32_t *dcbSize,
    void **ccb,
    uint32_t *ccbSize
)
{
    int result = HOOK_CONTINUE(
        sceGnmSubmitCommandBuffers,
        count, dcb, dcbSize, ccb, ccbSize
    );

    if (!g_probe_gnm_submit++)
    {
        final_printf(
            "[DGProbe] GNM submit count=%u result=%d\n",
            count, result
        );
    }

    return result;
}

static int probe_video_flip(
    int handle,
    int bufferIndex,
    int flipMode,
    int flipArg
)
{
    int result = HOOK_CONTINUE(
        sceVideoOutSubmitFlip,
        handle, bufferIndex, flipMode, flipArg
    );

    if (!g_probe_video_flip++)
    {
        final_printf(
            "[DGProbe] VideoOut flip handle=%d buffer=%d mode=%d arg=%d result=%d\n",
            handle, bufferIndex, flipMode, flipArg, result
        );
    }

    return result;
}

static int probe_video_register(
    int handle,
    int startIndex,
    void *buffers,
    int bufferNum,
    DaysGoneVideoOutAttr *attr
)
{
    int result = HOOK_CONTINUE(
        sceVideoOutRegisterBuffers,
        handle, startIndex, buffers, bufferNum, attr
    );

    if (!g_probe_video_register++)
    {
        if (attr)
        {
            final_printf(
                "[DGProbe] Register handle=%d start=%d count=%d width=%u height=%u format=%u pitch=%u result=%d\n",
                handle, startIndex, bufferNum,
                attr->width, attr->height,
                attr->format, attr->pitch, result
            );
        }
        else
        {
            final_printf(
                "[DGProbe] Register handle=%d count=%d attr=null result=%d\n",
                handle, bufferNum, result
            );
        }
    }

    return result;
}

void FrameProbe_Init(void)
{
    HOOK_INIT(sceGnmSubmitAndFlipCommandBuffers, probe_gnm_flip);
    HOOK_INIT(sceGnmSubmitCommandBuffers, probe_gnm_submit);
    HOOK_INIT(sceVideoOutSubmitFlip, probe_video_flip);
    HOOK_INIT(sceVideoOutRegisterBuffers, probe_video_register);

    final_printf("[DGProbe] Frame probe initialized\n");
}

void FrameProbe_Shutdown(void)
{
    HOOK32(sceGnmSubmitAndFlipCommandBuffers, HOOK_UNHOOK);
    HOOK32(sceGnmSubmitCommandBuffers, HOOK_UNHOOK);
    HOOK32(sceVideoOutSubmitFlip, HOOK_UNHOOK);
    HOOK32(sceVideoOutRegisterBuffers, HOOK_UNHOOK);

    final_printf("[DGProbe] Frame probe stopped\n");
}
