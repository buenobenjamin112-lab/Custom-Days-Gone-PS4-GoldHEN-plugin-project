
#include <Common.h>
#include <stdint.h>
#include <Detour.h>
#include "plugin_common.h"

/* Diagnóstico passivo: observa as chamadas e encaminha os argumentos originais. */

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

typedef int (*GnmFlipFn)(void *, uint32_t *, void *, uint32_t *, int, int, int, int);
typedef int (*GnmSubmitFn)(uint32_t, void **, uint32_t *, void **, uint32_t *);
typedef int (*VideoFlipFn)(int, int, int, int);
typedef int (*VideoRegisterFn)(int, int, void *, int, DaysGoneVideoOutAttr *);

extern int sceGnmSubmitAndFlipCommandBuffers(void *, uint32_t *, void *, uint32_t *, int, int, int, int);
extern int sceGnmSubmitCommandBuffers(uint32_t, void **, uint32_t *, void **, uint32_t *);
extern int sceVideoOutSubmitFlip(int, int, int, int);
extern int sceVideoOutRegisterBuffers(int, int, void *, int, DaysGoneVideoOutAttr *);

static Detour Detour_sceGnmSubmitAndFlipCommandBuffers = { DetourMode_x64 };
static Detour Detour_sceGnmSubmitCommandBuffers = { DetourMode_x64 };
static Detour Detour_sceVideoOutSubmitFlip = { DetourMode_x64 };
static Detour Detour_sceVideoOutRegisterBuffers = { DetourMode_x64 };

static GnmFlipFn g_originalGnmFlip = 0;
static GnmSubmitFn g_originalGnmSubmit = 0;
static VideoFlipFn g_originalVideoFlip = 0;
static VideoRegisterFn g_originalVideoRegister = 0;

static volatile int g_probe_gnm_flip = 0;
static volatile int g_probe_gnm_submit = 0;
static volatile int g_probe_video_flip = 0;
static volatile int g_probe_video_register = 0;
static int g_probe_initialized = 0;

static int sceGnmSubmitAndFlipCommandBuffers_hook(
    void *dcb, uint32_t *dcbSize, void *ccb, uint32_t *ccbSize,
    int handle, int bufferIndex, int flipMode, int flipArg)
{
    int result;
    if (!g_originalGnmFlip)
        return -1;

    result = g_originalGnmFlip(
        dcb, dcbSize, ccb, ccbSize,
        handle, bufferIndex, flipMode, flipArg);

    if (!g_probe_gnm_flip++)
        final_printf("[DGProbe] GNM flip handle=%d buffer=%d mode=%d arg=%d result=%d\n",
            handle, bufferIndex, flipMode, flipArg, result);

    return result;
}

static int sceGnmSubmitCommandBuffers_hook(
    uint32_t count, void **dcb, uint32_t *dcbSize,
    void **ccb, uint32_t *ccbSize)
{
    int result;
    if (!g_originalGnmSubmit)
        return -1;

    result = g_originalGnmSubmit(count, dcb, dcbSize, ccb, ccbSize);

    if (!g_probe_gnm_submit++)
        final_printf("[DGProbe] GNM submit count=%u result=%d\n", count, result);

    return result;
}

static int sceVideoOutSubmitFlip_hook(
    int handle, int bufferIndex, int flipMode, int flipArg)
{
    int result;
    if (!g_originalVideoFlip)
        return -1;

    result = g_originalVideoFlip(handle, bufferIndex, flipMode, flipArg);

    if (!g_probe_video_flip++)
        final_printf("[DGProbe] VideoOut flip handle=%d buffer=%d mode=%d arg=%d result=%d\n",
            handle, bufferIndex, flipMode, flipArg, result);

    return result;
}

static int sceVideoOutRegisterBuffers_hook(
    int handle, int startIndex, void *buffers,
    int bufferNum, DaysGoneVideoOutAttr *attr)
{
    int result;
    if (!g_originalVideoRegister)
        return -1;

    result = g_originalVideoRegister(handle, startIndex, buffers, bufferNum, attr);

    if (!g_probe_video_register++)
    {
        if (attr)
            final_printf("[DGProbe] Register handle=%d start=%d count=%d width=%u height=%u format=%u pitch=%u result=%d\n",
                handle, startIndex, bufferNum,
                attr->width, attr->height, attr->format, attr->pitch, result);
        else
            final_printf("[DGProbe] Register handle=%d count=%d attr=null result=%d\n",
                handle, bufferNum, result);
    }

    return result;
}

void FrameProbe_Shutdown(void);

int FrameProbe_Init(void)
{
    void *stub;

    if (g_probe_initialized)
        return 0;

    g_probe_gnm_flip = 0;
    g_probe_gnm_submit = 0;
    g_probe_video_flip = 0;
    g_probe_video_register = 0;

    Detour_Construct(&Detour_sceGnmSubmitAndFlipCommandBuffers, DetourMode_x32);
    stub = Detour_DetourFunction(&Detour_sceGnmSubmitAndFlipCommandBuffers,
        (uint64_t)sceGnmSubmitAndFlipCommandBuffers,
        (void *)sceGnmSubmitAndFlipCommandBuffers_hook);
    if (!stub)
        goto fail;
    g_originalGnmFlip = (GnmFlipFn)stub;

    Detour_Construct(&Detour_sceGnmSubmitCommandBuffers, DetourMode_x32);
    stub = Detour_DetourFunction(&Detour_sceGnmSubmitCommandBuffers,
        (uint64_t)sceGnmSubmitCommandBuffers,
        (void *)sceGnmSubmitCommandBuffers_hook);
    if (!stub)
        goto fail;
    g_originalGnmSubmit = (GnmSubmitFn)stub;

    Detour_Construct(&Detour_sceVideoOutSubmitFlip, DetourMode_x32);
    stub = Detour_DetourFunction(&Detour_sceVideoOutSubmitFlip,
        (uint64_t)sceVideoOutSubmitFlip,
        (void *)sceVideoOutSubmitFlip_hook);
    if (!stub)
        goto fail;
    g_originalVideoFlip = (VideoFlipFn)stub;

    Detour_Construct(&Detour_sceVideoOutRegisterBuffers, DetourMode_x32);
    stub = Detour_DetourFunction(&Detour_sceVideoOutRegisterBuffers,
        (uint64_t)sceVideoOutRegisterBuffers,
        (void *)sceVideoOutRegisterBuffers_hook);
    if (!stub)
        goto fail;
    g_originalVideoRegister = (VideoRegisterFn)stub;

    g_probe_initialized = 1;
    final_printf("[DGProbe] Frame probe initialized\n");
    return 0;

fail:
    FrameProbe_Shutdown();
    final_printf("[DGProbe] Failed to install frame probe hooks\n");
    return -1;
}

void FrameProbe_Shutdown(void)
{
    if (g_originalVideoRegister)
    {
        Detour_Destroy(&Detour_sceVideoOutRegisterBuffers);
        g_originalVideoRegister = 0;
    }

    if (g_originalVideoFlip)
    {
        Detour_Destroy(&Detour_sceVideoOutSubmitFlip);
        g_originalVideoFlip = 0;
    }

    if (g_originalGnmSubmit)
    {
        Detour_Destroy(&Detour_sceGnmSubmitCommandBuffers);
        g_originalGnmSubmit = 0;
    }

    if (g_originalGnmFlip)
    {
        Detour_Destroy(&Detour_sceGnmSubmitAndFlipCommandBuffers);
        g_originalGnmFlip = 0;
    }

    g_probe_initialized = 0;
}
