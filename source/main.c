
#include <stdint.h>
#include "plugin_common.h"

attr_public const char *g_pluginName = "DaysGoneSpawner";
attr_public const char *g_pluginDesc = "Days Gone test plugin";
attr_public const char *g_pluginAuth = "Custom";
attr_public uint32_t g_pluginVersion = 0x00000100;

int32_t attr_public plugin_load(int32_t argc, const char *argv[])
{
    final_printf("[DaysGoneSpawner] Plugin iniciado!\n");
    NotifyStatic(TEX_ICON_SYSTEM, "DaysGoneSpawner carregado!");
    return 0;
}

int32_t attr_public plugin_unload(int32_t argc, const char *argv[])
{
    final_printf("[DaysGoneSpawner] Plugin encerrado!\n");
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
