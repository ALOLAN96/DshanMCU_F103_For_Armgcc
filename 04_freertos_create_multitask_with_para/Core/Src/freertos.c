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
#include "driver_timer.h"
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
typedef struct {
    uint8_t x;
    uint8_t y;
    char buff[16];
} TaskPrintfInfo;

static TaskPrintfInfo g_Task1PrintInfo = {
    .x    = 0,
    .y    = 0,
    .buff = "Task1",
};

static TaskPrintfInfo g_Task2PrintInfo = {
    .x    = 0,
    .y    = 3,
    .buff = "Task2",
};

static TaskPrintfInfo g_Task3PrintInfo = {
    .x    = 0,
    .y    = 6,
    .buff = "Task3",
};

static bool g_PrintFlag = 1; // 打印标志
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
void LcdPrintTask(void *argument);
/* USER CODE END FunctionPrototypes */

void AppStartTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */
    TaskHandle_t AppTask1Handle, AppTask2Handle, AppTask3Handle;

    LCD_Init();
    LCD_Clear();
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
    xTaskCreate(LcdPrintTask, "Task1", 128, &g_Task1PrintInfo, osPriorityNormal, &AppTask1Handle);
    xTaskCreate(LcdPrintTask, "Task2", 128, &g_Task2PrintInfo, osPriorityNormal, &AppTask2Handle);
    xTaskCreate(LcdPrintTask, "Task3", 128, &g_Task3PrintInfo, osPriorityNormal, &AppTask3Handle);

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
    /* Infinite loop */
    for (;;) {
        Led_Test();
    }
    /* USER CODE END AppStartTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void LcdPrintTask(void *argument)
{
    TaskPrintfInfo *pInfo = (TaskPrintfInfo *)argument;

    uint32_t cnt = 0;
    int32_t len; // 记录打印长度

    for (;;) {
        if (g_PrintFlag) {
            g_PrintFlag = 0;
            len       = LCD_PrintString(pInfo->x, pInfo->y, pInfo->buff);
            len += LCD_PrintString(len, pInfo->y, ":");
            LCD_PrintSignedVal(len, pInfo->y, cnt++);
            g_PrintFlag = 1;
        }
        mdelay(500);
    }
}
/* USER CODE END Application */
