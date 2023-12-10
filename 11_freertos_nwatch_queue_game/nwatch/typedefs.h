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

#endif /* TYPEDEFS_H_ */
