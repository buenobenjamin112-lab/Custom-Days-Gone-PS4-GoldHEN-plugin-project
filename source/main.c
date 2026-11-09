
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <Common.h>
#include <orbis/Pad.h>
#include <orbis/UserService.h>
#include "plugin_common.h"

attr_public const char *g_pluginName = "DaysGoneSpawner";
attr_public const char *g_pluginDesc = "Days Gone Spawner";
attr_public const char *g_pluginAuth = "Custom";
attr_public uint32_t g_pluginVersion = 0x00000100;

static volatile bool running = false;
static bool menu_open = false;
static OrbisPthread input_thread;

static int get_pad_handle(void)
{
    int user_id = 0;
    OrbisUserServiceInitializeParams params = {0};
    params.priority = ORBIS_KERNEL_PRIO_FIFO_LOWEST;

    sceUserServiceInitialize(&params);

    if (sceUserServiceGetInitialUser(&user_id) < 0)
        return 0;

    int handle = scePadOpen(user_id, 0, 0, 0);

    if (handle == ORBIS_PAD_ERROR_ALREADY_OPENED)
        handle = scePadGetHandle(user_id, 0, 0);

    return handle > 0 ? handle : 0;
}

static void *input_loop(void *arg)
{
    (void)arg;

    sleep(3);

    if (scePadInit() < 0)
    {
        NotifyStatic(TEX_ICON_SYSTEM, "Erro ao iniciar controle");
        running = false;
        return NULL;
    }

    int handle = get_pad_handle();
    bool previous = false;

    while (running)
    {
        OrbisPadData data = {0};

        if (handle > 0 &&
            scePadReadState(handle, &data) == 0 &&
            data.connected)
        {
            bool pressed =
                (data.buttons & ORBIS_PAD_BUTTON_R1) &&
                (data.buttons & ORBIS_PAD_BUTTON_SQUARE);

            if (pressed && !previous)
            {
                menu_open = !menu_open;

                if (menu_open)
                    NotifyStatic(TEX_ICON_SYSTEM, "DaysGoneSpawner: menu aberto");
                else
                    NotifyStatic(TEX_ICON_SYSTEM, "DaysGoneSpawner: menu fechado");
            }

            previous = pressed;
        }

        usleep(50000);
    }

    return NULL;
}

int32_t attr_public plugin_load(int32_t argc, const char *argv[])
{
    (void)argc;
    (void)argv;

    running = true;

    NotifyStatic(TEX_ICON_SYSTEM, "DaysGoneSpawner iniciado");

    scePthreadCreate(
        &input_thread,
        NULL,
        input_loop,
        NULL,
        "DaysGoneSpawnerInput"
    );

    return 0;
}

int32_t attr_public plugin_unload(int32_t argc, const char *argv[])
{
    (void)argc;
    (void)argv;

    running = false;
    return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void *args)
{
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args)
{
    return 0;
}
