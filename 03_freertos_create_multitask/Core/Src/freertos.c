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
/* Definitions for taskOled */
osThreadId_t taskOledHandle;
const osThreadAttr_t taskOled_attributes = {
  .name = "taskOled",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for taskLed */
osThreadId_t taskLedHandle;
const osThreadAttr_t taskLed_attributes = {
  .name = "taskLed",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for taskColoerLed */
osThreadId_t taskColoerLedHandle;
const osThreadAttr_t taskColoerLed_attributes = {
  .name = "taskColoerLed",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for taskBuzzer */
osThreadId_t taskBuzzerHandle;
const osThreadAttr_t taskBuzzer_attributes = {
  .name = "taskBuzzer",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTaskOled(void *argument);
void AppTaskLed(void *argument);
void AppTaskColoerLed(void *argument);
void AppTaskBuzzer(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
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
  /* creation of taskOled */
  taskOledHandle = osThreadNew(AppTaskOled, NULL, &taskOled_attributes);

  /* creation of taskLed */
  taskLedHandle = osThreadNew(AppTaskLed, NULL, &taskLed_attributes);

  /* creation of taskColoerLed */
  taskColoerLedHandle = osThreadNew(AppTaskColoerLed, NULL, &taskColoerLed_attributes);

  /* creation of taskBuzzer */
  taskBuzzerHandle = osThreadNew(AppTaskBuzzer, NULL, &taskBuzzer_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTaskOled */
/**
  * @brief  Function implementing the TaskOled thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTaskOled */
void AppTaskOled(void *argument)
{
  /* USER CODE BEGIN AppTaskOled */
  /* Infinite loop */
  for(;;)
  {
      IRReceiver_Test();
  }
  /* USER CODE END AppTaskOled */
}

/* USER CODE BEGIN Header_AppTaskLed */
/**
* @brief Function implementing the TaskLed thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTaskLed */
void AppTaskLed(void *argument)
{
  /* USER CODE BEGIN AppTaskLed */
  /* Infinite loop */
  for(;;)
  {
      Led_Test();
  }
  /* USER CODE END AppTaskLed */
}

/* USER CODE BEGIN Header_AppTaskColoerLed */
/**
* @brief Function implementing the taskColoerLed thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTaskColoerLed */
void AppTaskColoerLed(void *argument)
{
  /* USER CODE BEGIN AppTaskColoerLed */
  /* Infinite loop */
  for(;;)
  {
      ColorLED_Test();
  }
  /* USER CODE END AppTaskColoerLed */
}

/* USER CODE BEGIN Header_AppTaskBuzzer */
/**
* @brief Function implementing the taskBuzzer thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppTaskBuzzer */
void AppTaskBuzzer(void *argument)
{
  /* USER CODE BEGIN AppTaskBuzzer */
  /* Infinite loop */
  for(;;)
  {
      PassiveBuzzer_Test();
  }
  /* USER CODE END AppTaskBuzzer */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

