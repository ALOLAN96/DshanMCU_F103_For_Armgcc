#include "beep.h"

static TimerHandle_t g_TimeSound;

void GameSoundTimer(TimerHandle_t xTimer)
{
    /* 停止蜂鸣器 */
    PassiveBuzzer_Control(0);
}

void BuzzerInit(void)
{
    /* 初始化蜂鸣器 */
    PassiveBuzzer_Init();

    /* 创建软件定时器 */
    g_TimeSound = xTimerCreate("GameSound", 200, pdFALSE, NULL, GameSoundTimer);
}

void BuzzerBuzz(int freq, int timeMs)
{
    PassiveBuzzer_Set_Freq_Duty(freq, 50);

    /* 启动定时器 */
    xTimerChangePeriod(g_TimeSound, timeMs, 0);
}