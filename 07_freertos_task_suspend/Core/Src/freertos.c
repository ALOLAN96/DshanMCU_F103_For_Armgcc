/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "driver_led.h"
#include "driver_lcd.h"
#include "driver_passive_buzzer.h"
#include "driver_ir_receiver.h"
#include "music.c"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for startTask */
osThreadId_t startTaskHandle;
const osThreadAttr_t startTask_attributes = {
    .name       = "startTask",
    .stack_size = 128 * 4,
    .priority   = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void AppStartTask(void *argument);
void PlayMusic(void *params);
/* USER CODE END FunctionPrototypes */

void TaskBuzzer(void *argument);
void TaskLed(void *argument);
void TaskColorLed(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of startTask */
    startTaskHandle = osThreadNew(AppStartTask, NULL, &startTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_AppStartTask */
/**
 * @brief  Function implementing the startTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_AppStartTask */
void AppStartTask(void *argument)
{
    /* USER CODE BEGIN AppStartTask */
    /* Oled 初始化 */
    LCD_Init();
    LCD_Clear();
    LCD_PrintString(0, 0, "Waiting Control");

    /* 红外接收传感器初始化 */
    IRReceiver_Init();

    uint8_t dev, data;
    bool bRunning = 1;
    int len;
    TaskHandle_t xBuzzerTaskHandle, xOledTaskhandle, xLedTaskhandle;
    xBuzzerTaskHandle = xOledTaskhandle = xLedTaskhandle = NULL;

    /* 创建光任务 */
    xTaskCreate(TaskLed, "Oled Task", 128, NULL, osPriorityNormal, &xOledTaskhandle);

    /* Infinite loop */
    for (;;) {
        if (0u == IRReceiver_Read(&dev, &data)) {
            if (data == 0xa8u) {
                /* Play 按键按下 */
                /* 创建声任务 */
                if (xBuzzerTaskHandle == NULL) {
                    LCD_ClearLine(0, 0);
                    LCD_PrintString(0, 0, "Create to Music");
                    xTaskCreate(PlayMusic, "Buzzer Task", 128, NULL, osPriorityAboveNormal1, &xBuzzerTaskHandle);
                } else {
                    if (bRunning) {
                        LCD_ClearLine(0, 0);
                        LCD_PrintString(0, 0, "Suspend Music");
                        vTaskSuspend(xBuzzerTaskHandle);
                        PassiveBuzzer_Control(0); // 蜂鸣器仍按设定的工作，需要手动停止
                        bRunning = 0;
                    } else {
                        LCD_ClearLine(0, 0);
                        LCD_PrintString(0, 0, "Resume Music");
                        vTaskResume(xBuzzerTaskHandle);
                        bRunning = 1;
                    }
                }

            } else if (data == 0xa2u) {
                if (xBuzzerTaskHandle != NULL) {
                    /* 删除声任务 */
                    LCD_ClearLine(0, 0);
                    LCD_PrintString(0, 0, "Delete Task");
                    vTaskDelete(xBuzzerTaskHandle);
                    PassiveBuzzer_Control(0); // 任务虽然删除，但是蜂鸣器仍按设定的工作，需要手动停止
                    xBuzzerTaskHandle = NULL;
                }
            }

            len = LCD_PrintString(0, 6, "Key name: ");
            LCD_PrintString(len, 6, IRReceiver_CodeToString(data));
        }
    }
    /* USER CODE END AppStartTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void TaskLed(void *argument)
{
    for (;;) {
        Led_Test();
    }
}

void MUSIC_Analysis(void)
{
    uint16_t MusicBeatNum = ((((sizeof(Music_Lone_Brave)) / 2) / 3) - 1);

    uint16_t MusicSpeed = Music_Lone_Brave[0][2];
    for (uint16_t i = 1; i <= MusicBeatNum; i++) {
        // BSP_Buzzer_SetFrequency(Tone_Index[Music_Lone_Brave[i][0]][Music_Lone_Brave[i][1]]);
        PassiveBuzzer_Set_Freq_Duty(Tone_Index[Music_Lone_Brave[i][0]][Music_Lone_Brave[i][1]], 50);
        // HAL_Delay(MusicSpeed/Music_Lone_Brave[i][2]);
        // mdelay(MusicSpeed / Music_Lone_Brave[i][2]);
        vTaskDelay(MusicSpeed / Music_Lone_Brave[i][2]);
    }
}
/* USER CODE END Application */
