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
#include "driver_oled.h"
#include "driver_passive_buzzer.h"
#include "driver_color_led.h"
#include "driver_ir_receiver.h"
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
void MUSIC_Analysis(void);

void AppStartTask(void *argument);
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
    TaskHandle_t xBuzzerTaskHandle, xOledTaskhandle, xLedTaskhandle;

    /* 创建声任务 */
    xTaskCreate(TaskBuzzer, "Buzzer Task", 128, NULL, osPriorityNormal, &xBuzzerTaskHandle);

    /* 创建光任务 */
    xTaskCreate(TaskLed, "Oled Task", 128, NULL, osPriorityNormal, &xOledTaskhandle);

    /* 创建色任务 */
    xTaskCreate(TaskColorLed, "Led Task", 128, NULL, osPriorityNormal, &xLedTaskhandle);

    /* Infinite loop */
    for (;;) {
        IRReceiver_Test();
    }
    /* USER CODE END AppStartTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void TaskBuzzer(void *argument)
{
    for (;;) {
        MUSIC_Analysis();
    }
}

void TaskLed(void *argument)
{
    for (;;) {
        Led_Test();
    }
}

void TaskColorLed(void *argument)
{
    for (;;) {
        ColorLED_Test();
    }
}

/* USER CODE END Application */
