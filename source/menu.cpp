#include "menu.hpp"

#define MENU_OPTION_COUNT 3

static DaysGoneMenu g_menu = { false, 0 };

extern "C"
{

void Menu_Init(void)
{
    g_menu.visible = false;
    g_menu.selected = 0;
}

void Menu_Toggle(void)
{
    g_menu.visible = !g_menu.visible;
}

void Menu_Move(int direction)
{
    if (!g_menu.visible || direction == 0)
        return;

    g_menu.selected += direction;

    if (g_menu.selected < 0)
        g_menu.selected = MENU_OPTION_COUNT - 1;

    if (g_menu.selected >= MENU_OPTION_COUNT)
        g_menu.selected = 0;
}

void Menu_Select(void)
{
    if (!g_menu.visible)
        return;

    // As ações do menu serão implementadas depois.
}

bool Menu_IsVisible(void)
{
    return g_menu.visible;
}

int Menu_GetSelected(void)
{
    return g_menu.selected;
}

}
