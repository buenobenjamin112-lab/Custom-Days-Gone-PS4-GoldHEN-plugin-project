
#ifndef DAYS_GONE_PAD_H
#define DAYS_GONE_PAD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int PadController_Init(void);
void PadController_Update(void);
void PadController_Shutdown(void);

int PadController_IsReady(void);
int PadController_IsConnected(void);
uint32_t PadController_GetButtons(void);

#ifdef __cplusplus
}
#endif

#endif
