
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "plugin_common.h"
#include "Common.h"
#include <Patcher.h>
#include "pad.h"

attr_public const char* g_pluginName = "DaysGoneSpawner";
attr_public const char* g_pluginDesc = "Days Gone Controller Hook";
attr_public const char* g_pluginAuth = "Custom";
attr_public uint32_t g_pluginVersion = 0x00000100;

HOOK_INIT(scePadRead);
HOOK_INIT(scePadReadState);

static Patcher* readPatcher = NULL;
static Patcher* readStatePatcher = NULL;

static volatile bool menuOpen = false;
static volatile bool previousCombo = false;

static void process_menu_input(ScePadData* data)
{
    if (data == NULL)
        return;

    const uint32_t combo =
        SCE_PAD_BUTTON_R1 | SCE_PAD_BUTTON_SQUARE;

    bool pressed = (data->buttons & combo) == combo;

    if (pressed && !previousCombo)
    {
        menuOpen = !menuOpen;

        if (menuOpen)
            NotifyStatic(TEX_ICON_SYSTEM,
                         "DaysGoneSpawner: menu aberto");
        else
            NotifyStatic(TEX_ICON_SYSTEM,
                         "DaysGoneSpawner: menu fechado");
    }

    previousCombo = pressed;
}

int32_t scePadRead_hook(
    int32_t handle,
    ScePadData* data,
    int32_t count)
{
    int ret = scePadReadExt(handle, data, count);

    if (ret <= 0 || data == NULL)
        return ret;

    for (int i = 0; i < ret; i++)
        process_menu_input(&data[i]);

    return ret;
}

int32_t scePadReadState_hook(
    int32_t handle,
    ScePadData* data)
{
    int ret = scePadReadStateExt(handle, data);

    if (ret != 0 || data == NULL)
        return ret;

    process_menu_input(data);

    return ret;
}

s32 attr_public plugin_load(s32 argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    NotifyStatic(TEX_ICON_SYSTEM,
                 "DaysGoneSpawner iniciado");

    char module[256];
    snprintf(
        module,
        sizeof(module),
        "/%s/common/lib/libScePad.sprx",
        sceKernelGetFsSandboxRandomWord()
    );

    int handle = 0;
    sys_dynlib_load_prx(module, &handle);

    readPatcher = malloc(sizeof(Patcher));
    readStatePatcher = malloc(sizeof(Patcher));

    if (!readPatcher || !readStatePatcher)
        return -1;

    Patcher_Construct(readPatcher);
    Patcher_Construct(readStatePatcher);

    uint8_t patchRead[5] =
        {0x31, 0xC9, 0x90, 0x90, 0x90};

    uint8_t patchReadState[5] =
        {0x31, 0xD2, 0x90, 0x90, 0x90};

    Patcher_Install_Patch(
        readPatcher,
        (uint64_t)scePadReadExt,
        patchRead,
        sizeof(patchRead)
    );

    Patcher_Install_Patch(
        readStatePatcher,
        (uint64_t)scePadReadStateExt,
        patchReadState,
        sizeof(patchReadState)
    );

    HOOK32(scePadRead);
    HOOK32(scePadReadState);

    return 0;
}

s32 attr_public plugin_unload(s32 argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    UNHOOK(scePadRead);
    UNHOOK(scePadReadState);

    if (readPatcher)
    {
        Patcher_Destroy(readPatcher);
        free(readPatcher);
        readPatcher = NULL;
    }

    if (readStatePatcher)
    {
        Patcher_Destroy(readStatePatcher);
        free(readStatePatcher);
        readStatePatcher = NULL;
    }

    return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void* args)
{
    (void)argc;
    (void)args;
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void* args)
{
    (void)argc;
    (void)args;
    return 0;
}
