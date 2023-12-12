/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdbool.h>
#include <stdint.h>

#include "cmsis_os.h"
#include "FreeRTOS.h"     // ARM.FreeRTOS::RTOS:Core
#include "task.h"         // ARM.FreeRTOS::RTOS:Core
#include "queue.h"        // ARM.FreeRTOS::RTOS:Queue
#include "event_groups.h" // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"       // ARM.FreeRTOS::RTOS:Semphr

typedef uint8_t byte;
typedef uint16_t uint;
typedef uint32_t ulong;

#define PROGMEM

// Quick and easy macro for a for loop
#define LOOP(count, var) for (byte var = 0; var < count; var++)

typedef struct {
    uint32_t dev;
    uint32_t val;
} IrRecvData; // 红外接收器消息队列item

typedef struct {
    int32_t cnt;
    int32_t speed;
} RotaryRecvData; // 旋转编码器消息队列item

typedef struct
{
    int32_t dev;
    int32_t val;
} PlatformRecvData; // 挡球板消息队列item

#define IR_QUEUE_LEN     (10u)
#define ROTARY_QUEUE_LEN (10u)

typedef enum {
    POWER    = 0xa2,
    MENU     = 0xe2,
    TEST     = 0x22,
    INCREASE = 0x02,
    RETURN   = 0xc2,
    LEFT     = 0xe0,
    PLAY     = 0xa8,
    RIGHT    = 0x90,
    NUM0     = 0x68,
    REDUCE   = 0x98,
    LETTERC  = 0xb0,
    NUM1     = 0x30,
    NUM2     = 0x18,
    NUM3     = 0x7a,
    NUM4     = 0x10,
    NUM5     = 0x38,
    NUM6     = 0x5a,
    NUM7     = 0x42,
    NUM8     = 0x4a,
    NUM9     = 0x5a,
    REPEAT   = 0x00,
} IrRecvKey;

typedef enum 
{
    UPT_MOVE_NONE,
    UPT_MOVE_RIGHT,
    UPT_MOVE_LEFT
}PlatformCMD;

QueueHandle_t GetIrQueueHandle(void);
QueueHandle_t GetRotaryQueueHandle(void);

#endif /* TYPEDEFS_H_ */
