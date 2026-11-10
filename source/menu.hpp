#ifndef DAYS_GONE_MENU_HPP
#define DAYS_GONE_MENU_HPP

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    bool visible;
    int selected;
} DaysGoneMenu;

void Menu_Init(void);
void Menu_Toggle(void);
void Menu_Move(int direction);
void Menu_Select(void);
bool Menu_IsVisible(void);
int Menu_GetSelected(void);

#ifdef __cplusplus
}
#endif

#endif
