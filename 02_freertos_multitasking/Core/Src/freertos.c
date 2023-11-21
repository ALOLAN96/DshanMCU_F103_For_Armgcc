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
/* Definitions for TaskOled */
osThreadId_t TaskOledHandle;
const osThreadAttr_t TaskOled_attributes = {
  .name = "TaskOled",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskLed */
osThreadId_t TaskLedHandle;
const osThreadAttr_t TaskLed_attributes = {
  .name = "TaskLed",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTaskOled(void *argument);
void AppTaskLed(void *argument);

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
  /* creation of TaskOled */
  TaskOledHandle = osThreadNew(AppTaskOled, NULL, &TaskOled_attributes);

  /* creation of TaskLed */
  TaskLedHandle = osThreadNew(AppTaskLed, NULL, &TaskLed_attributes);

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
      OLED_Test();
      osDelay(1);
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
      osDelay(1);
  }
  /* USER CODE END AppTaskLed */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

