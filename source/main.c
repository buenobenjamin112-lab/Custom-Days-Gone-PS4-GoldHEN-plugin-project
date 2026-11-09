#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "plugin_common.h"

/*
 * DaysGoneSpawner - etapa 1
 * Este plugin e SOMENTE um teste de carregamento no GoldHEN.
 * Ainda nao chama funcoes internas de Days Gone e nao cria hordas.
 */

attr_public const char *g_pluginName = "DaysGoneSpawner";
attr_public const char *g_pluginDesc = "Days Gone Spawner - loader test";
attr_public const char *g_pluginAuth = "Community project";
attr_public uint32_t g_pluginVersion = 0x00000100; /* 1.00 */

int32_t attr_public plugin_load(int32_t argc, const char *argv[])
{
    (void)argc;
    (void)argv;

    final_printf("[DaysGoneSpawner] Plugin carregado. Etapa 1: teste seguro.\n");
    NotifyStatic(TEX_ICON_SYSTEM, "DaysGoneSpawner: plugin carregado");
    return 0;
}

int32_t attr_public plugin_unload(int32_t argc, const char *argv[])
{
    (void)argc;
    (void)argv;

    final_printf("[DaysGoneSpawner] Plugin descarregado.\n");
    return 0;
}

s32 attr_module_hidden module_start(s64 argc, const void *args)
{
    (void)argc;
    (void)args;
    return 0;
}

s32 attr_module_hidden module_stop(s64 argc, const void *args)
{
    (void)argc;
    (void)args;
    return 0;
}
