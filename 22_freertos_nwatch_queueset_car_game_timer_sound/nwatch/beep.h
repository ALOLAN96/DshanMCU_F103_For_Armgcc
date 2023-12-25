#ifndef __BEEP_H__
#define __BEEP_H__

#include "typedefs.h"
#include "driver_passive_buzzer.h"

void GameSoundTimer(TimerHandle_t xTimer);

void BuzzerInit(void);

void BuzzerBuzz(int freq, int timeMs);

#endif // !__BEEP_H__