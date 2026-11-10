
#include "pad.h"
#include "menu.hpp"
#include "plugin_common.h"

#include <stdint.h>
#include <orbis/Pad.h>
#include <Detour.h>

static Detour g_readDetour;
static Detour g_readStateDetour;

static bool g_initialized = false;
static uint32_t g_previousButtons = 0;
static bool g_previousCombo = false;

typedef int32_t (*PadReadFn)(
    int32_t,
    OrbisPadData*,
    int32_t
);

typedef int32_t (*PadReadStateFn)(
    int32_t,
    OrbisPadData*
);

static PadReadFn g_originalRead = nullptr;
static PadReadStateFn g_originalReadState = nullptr;

static void ProcessMenuInput(OrbisPadData* data)
{
    if (!data)
        return;

    uint32_t buttons = data->buttons;

    const uint32_t combo =
        ORBIS_PAD_BUTTON_R1 |
        ORBIS_PAD_BUTTON_SQUARE;

    bool comboPressed = (buttons & combo) == combo;

    if (comboPressed && !g_previousCombo)
        Menu_Toggle();

    g_previousCombo = comboPressed;

    if (Menu_IsVisible())
    {
        if ((buttons & ORBIS_PAD_BUTTON_UP) &&
            !(g_previousButtons & ORBIS_PAD_BUTTON_UP))
            Menu_Move(-1);

        if ((buttons & ORBIS_PAD_BUTTON_DOWN) &&
            !(g_previousButtons & ORBIS_PAD_BUTTON_DOWN))
            Menu_Move(1);

        if ((buttons & ORBIS_PAD_BUTTON_CROSS) &&
            !(g_previousButtons & ORBIS_PAD_BUTTON_CROSS))
            Menu_Select();

        if ((buttons & ORBIS_PAD_BUTTON_CIRCLE) &&
            !(g_previousButtons & ORBIS_PAD_BUTTON_CIRCLE))
            Menu_Toggle();
    }

    g_previousButtons = buttons;
}

static int32_t PadReadHook(
    int32_t handle,
    OrbisPadData* data,
    int32_t count)
{
    if (!g_originalRead)
        return -1;

    int32_t result = g_originalRead(handle, data, count);

    if (result > 0 && data)
    {
        for (int32_t i = 0; i < result; i++)
            ProcessMenuInput(&data[i]);
    }

    return result;
}

static int32_t PadReadStateHook(
    int32_t handle,
    OrbisPadData* data)
{
    if (!g_originalReadState)
        return -1;

    int32_t result = g_originalReadState(handle, data);

    if (result == 0 && data)
        ProcessMenuInput(data);

    return result;
}

extern "C"
{

int PadController_Init(void)
{
    if (g_initialized)
        return 0;

    g_previousButtons = 0;
    g_previousCombo = false;

    Detour_Construct(&g_readDetour, DetourMode_x32);

    void* readStub = Detour_DetourFunction(
        &g_readDetour,
        (uint64_t)scePadRead,
        (void*)PadReadHook
    );

    if (!readStub)
    {
        final_printf("[DaysGoneSpawner] Falha no hook scePadRead.\n");
        return -1;
    }

    g_originalRead = (PadReadFn)readStub;

    Detour_Construct(&g_readStateDetour, DetourMode_x32);

    void* stateStub = Detour_DetourFunction(
        &g_readStateDetour,
        (uint64_t)scePadReadState,
        (void*)PadReadStateHook
    );

    if (!stateStub)
    {
        final_printf("[DaysGoneSpawner] Falha no hook scePadReadState.\n");

        Detour_Destroy(&g_readDetour);
        g_originalRead = nullptr;
        return -2;
    }

    g_originalReadState = (PadReadStateFn)stateStub;

    g_initialized = true;

    final_printf("[DaysGoneSpawner] Hooks do controle instalados.\n");

    return 0;
}

void PadController_Update(void)
{
}

void PadController_Shutdown(void)
{
    if (!g_initialized)
        return;

    Detour_Destroy(&g_readStateDetour);
    Detour_Destroy(&g_readDetour);

    g_originalRead = nullptr;
    g_originalReadState = nullptr;

    g_previousButtons = 0;
    g_previousCombo = false;

    g_initialized = false;
}

int PadController_IsReady(void)
{
    return g_initialized ? 1 : 0;
}

int PadController_IsConnected(void)
{
    return g_initialized ? 1 : 0;
}

uint32_t PadController_GetButtons(void)
{
    return g_previousButtons;
}

}
