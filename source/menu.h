
#ifndef DAYS_GONE_MENU_H
#define DAYS_GONE_MENU_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool visible;
    int selected;
} DaysGoneMenu;

void Menu_Init(void);
void Menu_Toggle(void);
void Menu_Move(int direction);
void Menu_Select(void);
bool Menu_IsVisible(void);
int Menu_GetSelected(void);

#endif

