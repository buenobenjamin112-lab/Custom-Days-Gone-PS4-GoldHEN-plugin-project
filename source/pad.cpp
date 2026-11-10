#include "pad.h"
#include "menu.hpp"

#include <orbis/libkernel.h>
#include <orbis/_types/pad.h>
#include <orbis/_types/user.h>

static int g_padHandle = -1;
static OrbisPthread g_padThread;
static volatile bool g_padRunning = false;
static bool g_padThreadCreated = false;

static uint32_t g_previousButtons = 0;

static bool ButtonPressed(uint32_t buttons, uint32_t button)
{
    return (buttons & button) && !(g_previousButtons & button);
}

static void* PadThread(void* arg)
{
    (void)arg;

    OrbisPadData data = {};

    while (g_padRunning)
    {
        if (g_padHandle >= 0 &&
            scePadReadState(g_padHandle, &data) == 0 &&
            data.connected)
        {
            uint32_t buttons = data.buttons;

            const uint32_t menuShortcut =
                ORBIS_PAD_BUTTON_OPTIONS |
                ORBIS_PAD_BUTTON_L3 |
                ORBIS_PAD_BUTTON_R3 |
                ORBIS_PAD_BUTTON_L1 |
                ORBIS_PAD_BUTTON_R1;

            if ((buttons & menuShortcut) == menuShortcut &&
                (g_previousButtons & menuShortcut) != menuShortcut)
            {
                Menu_Toggle();
            }

            if (Menu_IsVisible())
            {
                if (ButtonPressed(buttons, ORBIS_PAD_BUTTON_UP))
                    Menu_Move(-1);

                if (ButtonPressed(buttons, ORBIS_PAD_BUTTON_DOWN))
                    Menu_Move(1);

                if (ButtonPressed(buttons, ORBIS_PAD_BUTTON_CROSS))
                    Menu_Select();

                if (ButtonPressed(buttons, ORBIS_PAD_BUTTON_CIRCLE))
                    Menu_Toggle();
            }

            g_previousButtons = buttons;
        }

        sceKernelUsleep(20000);
    }

    return 0;
}

extern "C"
{

int PadController_Init(void)
{
    if (g_padThreadCreated)
        return 0;

    int result = scePadInit();

    if (result < 0)
        return result;

    OrbisUserServiceInitializeParams params = {};
    params.priority = ORBIS_KERNEL_PRIO_FIFO_LOWEST;

    result = sceUserServiceInitialize(&params);

    if (result < 0)
        return result;

    int userID = -1;

    result = sceUserServiceGetInitialUser(&userID);

    if (result < 0)
        return result;

    g_padHandle = scePadOpen(userID, 0, 0, 0);

    if (g_padHandle < 0)
        return g_padHandle;

    g_previousButtons = 0;
    g_padRunning = true;

    result = scePthreadCreate(
        &g_padThread,
        NULL,
        PadThread,
        NULL,
        "DaysGonePad"
    );

    if (result != 0)
    {
        g_padRunning = false;
        scePadClose(g_padHandle);
        g_padHandle = -1;
        return result;
    }

    g_padThreadCreated = true;
    return 0;
}

void PadController_Update(void)
{
    // A leitura do controle acontece na thread.
}

void PadController_Shutdown(void)
{
    g_padRunning = false;

    if (g_padThreadCreated)
    {
        scePthreadJoin(g_padThread, NULL);
        g_padThreadCreated = false;
    }

    if (g_padHandle >= 0)
    {
        scePadClose(g_padHandle);
        g_padHandle = -1;
    }
}

int PadController_IsReady(void)
{
    return g_padThreadCreated ? 1 : 0;
}

int PadController_IsConnected(void)
{
    return g_padHandle >= 0 ? 1 : 0;
}

uint32_t PadController_GetButtons(void)
{
    return g_previousButtons;
}

}
