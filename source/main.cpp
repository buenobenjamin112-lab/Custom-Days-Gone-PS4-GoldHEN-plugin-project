
#include <stdint.h>
#include "plugin_common.h"
#include "menu.hpp"
#include "pad.h"

attr_public const char* g_pluginName = "DaysGoneSpawner";
attr_public const char* g_pluginDesc = "Days Gone C++ Plugin";
attr_public const char* g_pluginAuth = "Custom";
attr_public uint32_t g_pluginVersion = 0x00000104;

extern "C"
{

int32_t attr_public plugin_load(int32_t argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    Menu_Init();

    int padResult = PadController_Init();

    if (padResult < 0)
    {
        final_printf("[DaysGoneSpawner] Erro ao inicializar controle: %d\n", padResult);
    }
    else
    {
        final_printf("[DaysGoneSpawner] Controle inicializado.\n");
    }

    final_printf("[DaysGoneSpawner] Plugin iniciado.\n");

    return 0;
}

int32_t attr_public plugin_unload(int32_t argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    PadController_Shutdown();

    final_printf("[DaysGoneSpawner] Plugin descarregado.\n");
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

}
