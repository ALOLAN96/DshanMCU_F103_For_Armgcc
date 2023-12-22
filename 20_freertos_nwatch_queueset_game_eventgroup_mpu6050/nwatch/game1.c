/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "typedefs.h"

#include "draw.h"
#include "resources.h"

#include "driver_lcd.h"
#include "driver_ir_receiver.h"
#include "driver_mpu6050.h"

#define NOINVERT        false
#define INVERT          true

#define sprintf_P       sprintf
#define PSTR(a)         a

#define PLATFORM_WIDTH  12
#define PLATFORM_HEIGHT 4
#define BLOCK_COLS      32
#define BLOCK_ROWS      5
#define BLOCK_COUNT     (BLOCK_COLS * BLOCK_ROWS)

typedef struct {
    float x;
    float y;
    float velX;
    float velY;
} s_ball;

static const byte block[] = {
    0x07,
    0x07,
    0x07,
};

static const byte platform[] = {
    0x60,
    0x70,
    0x50,
    0x10,
    0x30,
    0xF0,
    0xF0,
    0x30,
    0x10,
    0x50,
    0x70,
    0x60,
};

static const byte ballImg[] = {
    0x03,
    0x03,
};

static const byte clearImg[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

static bool btnExit(void);
void game1_draw(void);

static byte uptMove;
static s_ball ball;
static bool *blocks;
static byte lives, lives_origin;
static uint score;
static byte platformX;

static uint32_t g_xres, g_yres, g_bpp;
static uint8_t *g_framebuffer;

static QueueSetHandle_t g_xQueueSetInput; /* 输入设备队列集，处理红外队列和旋转编码器队列 */
static EventGroupHandle_t g_xMPU6050EventGroupHandle;

static QueueHandle_t g_xQueuePlatform;      // 挡球板队列
static QueueHandle_t g_xQueueIrHandle;      // 红外队列
static QueueHandle_t g_xQueueRotaryHandle;  // 旋转编码器队列
static QueueHandle_t g_xQueueMPU6050Handle; // MPU6050队列

/* 挡球板任务 */
static void platform_task(void *params)
{
    byte platformXtmp = platformX;
    IrRecvData irRecvData;

    // Draw platform
    draw_bitmap(platformXtmp, g_yres - 8, platform, 12, 8, NOINVERT, 0);
    draw_flushArea(platformXtmp, g_yres - 8, 12, 8);

    while (1) {
        /* 读取红外遥控器 */
        // 从消息队列中拿取消息，拿不到就阻塞
        xQueueReceive(g_xQueuePlatform, &irRecvData, portMAX_DELAY);
        uptMove = irRecvData.val;

        // Hide platform
        draw_bitmap(platformXtmp, g_yres - 8, clearImg, 12, 8, NOINVERT, 0);
        draw_flushArea(platformXtmp, g_yres - 8, 12, 8);

        // Move platform
        if (uptMove == UPT_MOVE_RIGHT)
            platformXtmp += 3;
        else if (uptMove == UPT_MOVE_LEFT)
            platformXtmp -= 3;
        uptMove = UPT_MOVE_NONE;

        // Make sure platform stays on screen
        if (platformXtmp > 250)
            platformXtmp = 0;
        else if (platformXtmp > g_xres - PLATFORM_WIDTH)
            platformXtmp = g_xres - PLATFORM_WIDTH;

        // Draw platform
        draw_bitmap(platformXtmp, g_yres - 8, platform, 12, 8, NOINVERT, 0);
        draw_flushArea(platformXtmp, g_yres - 8, 12, 8);

        platformX = platformXtmp;
    }
}

static void ProcessIrData(void)
{
    IrRecvData irRecvData;
    static PlatformRecvData platformRecvData;
    xQueueReceive(g_xQueueIrHandle, &irRecvData, 0);

    if (irRecvData.val == LEFT) {
        platformRecvData.dev = irRecvData.dev;
        platformRecvData.val = UPT_MOVE_LEFT;
    } else if (irRecvData.val == RIGHT) {
        platformRecvData.dev = irRecvData.dev;
        platformRecvData.val = UPT_MOVE_RIGHT;
    } else if (irRecvData.val == REPEAT) {
        /* 重复值 */

    } else {
        platformRecvData.dev = irRecvData.dev;
        platformRecvData.val = UPT_MOVE_NONE;
    }

    xQueueSendToBack(g_xQueuePlatform, &platformRecvData, 0);
}

static void ProcessRotaryData(void)
{
    RotaryRecvData rotaryRecvData;
    PlatformRecvData platformRecvData;
    platformRecvData.dev = 1; // 对于platform来说，旋转编码器的dev无用
    int cnt;
    xQueueReceive(g_xQueueRotaryHandle, &rotaryRecvData, 0);

    if (rotaryRecvData.speed < 0) {           // 负数bug，此处教程是在这面做了处理
        platformRecvData.val = UPT_MOVE_LEFT; /* left的红外键值 */
    } else {
        platformRecvData.val = UPT_MOVE_RIGHT; /* right的红外键值 */
    }

    if (abs(rotaryRecvData.speed) > 100)
        cnt = 4;
    else if (abs(rotaryRecvData.speed) > 50) // 负数bug，此处教程是在上面做了处理
        cnt = 2;
    else
        cnt = 1;

    // 写挡球板队列
    for (size_t i = 0; i < cnt; i++) {
        xQueueSendToBack(g_xQueuePlatform, &platformRecvData, 0);
    }
}

static void ProcessMPU6050Data(void)
{
    PlatformRecvData platformRecvData;
    MPU6050RecvData mpu6050RecvData;
    platformRecvData.dev = 2; // 对于platform来说，dev无用

    xQueueReceive(g_xQueueMPU6050Handle, &mpu6050RecvData, 0);

    if (mpu6050RecvData.angle_x > 90) {
        platformRecvData.val = UPT_MOVE_LEFT;
    } else if (mpu6050RecvData.angle_x < 90) {
        platformRecvData.val = UPT_MOVE_RIGHT;
    } else {
        platformRecvData.val = UPT_MOVE_NONE;
    }

    xQueueSendToBack(g_xQueuePlatform, &platformRecvData, 0);
}

static void InputTask(void *params)
{
    QueueSetMemberHandle_t xRcvQueueHandle;

    for (;;) {
        // 读队列集，得到有数据的队列句柄
        xRcvQueueHandle = xQueueSelectFromSet(g_xQueueSetInput, portMAX_DELAY);

        if (xRcvQueueHandle == g_xQueueIrHandle) { // 数据处理
            ProcessIrData();
        } else if (xRcvQueueHandle == g_xQueueRotaryHandle) {
            ProcessRotaryData();
        } else if (xRcvQueueHandle == g_xQueueMPU6050Handle) {
            ProcessMPU6050Data();
        }
    }
}

extern void GetI2C(void);
extern void PutI2C(void);
extern EventGroupHandle_t GetMPU6050EventGroupHandle(void);

void MPU6050Task(void *params) // 因为此任务中数据不是由中断发送过来，而是自行创建个任务进行读取
{
    int16_t AccX;
    MPU6050RecvData mpu6050RecvData;
    g_xMPU6050EventGroupHandle = GetMPU6050EventGroupHandle();
    int res;
    for (;;) {
        // 等待事件bit0 
        xEventGroupWaitBits(g_xMPU6050EventGroupHandle, (1 << 0), pdTRUE, pdFALSE, portMAX_DELAY);

        GetI2C();
        res = MPU6050_ReadData(&AccX, NULL, NULL, NULL, NULL, NULL);
        PutI2C();

        if (0 == res) { // 读到数据
            // 解析数据
            MPU6050_ParseData(AccX, 0, 0, 0, 0, 0, &mpu6050RecvData);

            // 写入队列中
            xQueueSendToBack(g_xQueueMPU6050Handle, &mpu6050RecvData, 0);
        }
        vTaskDelay(50);
    }
}

void game1_task(void *params)
{
    g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
    draw_init();
    draw_end();

    // 创建消息队列集、消息队列、输入解码任务、MPU6050任务
    g_xQueueSetInput = xQueueCreateSet(IR_QUEUE_LEN + ROTARY_QUEUE_LEN); // 队列集长度应设置为添加到集合中的队列长度的总和
    g_xQueuePlatform = xQueueCreate(IR_QUEUE_LEN + ROTARY_QUEUE_LEN + MPU6050_QUEUE_LEN, sizeof(PlatformRecvData));
    xTaskCreate(InputTask, "InputTask", 128, NULL, osPriorityNormal, NULL);

    // 添加队列至队列集
    g_xQueueIrHandle      = GetIrQueueHandle();
    g_xQueueRotaryHandle  = GetRotaryQueueHandle();
    g_xQueueMPU6050Handle = GetMPU6050QueueHandle();
    xQueueAddToSet(g_xQueueIrHandle, g_xQueueSetInput);
    xQueueAddToSet(g_xQueueRotaryHandle, g_xQueueSetInput);
    xQueueAddToSet(g_xQueueMPU6050Handle, g_xQueueSetInput);

    xTaskCreate(MPU6050Task, "MPU6050Task", 128, NULL, osPriorityNormal, NULL); // 此处创建任务的原因是：先放入队列集，任务创建后就有数据，若未放入，队列满了，队列集不会出数据

    uptMove = UPT_MOVE_NONE;

    ball.x = g_xres / 2;
    ball.y = g_yres - 10;

    ball.velX = -0.5;
    ball.velY = -0.6;
    //	ball.velX = -1;
    //	ball.velY = -1.1;

    blocks = pvPortMalloc(BLOCK_COUNT);
    memset(blocks, 0, BLOCK_COUNT);

    lives = lives_origin = 3;
    score                = 0;
    platformX            = (g_xres / 2) - (PLATFORM_WIDTH / 2);

    xTaskCreate(platform_task, "platform_task", 128, NULL, osPriorityNormal, NULL);

    while (1) {
        game1_draw();
        // draw_end();
        vTaskDelay(50);
    }
}

static bool btnExit()
{

    vPortFree(blocks);
    if (lives == 255) {
        // game1_start();
    } else {
        // pwrmgr_setState(PWR_ACTIVE_DISPLAY, PWR_STATE_NONE);
        // animation_start(display_load, ANIM_MOVE_OFF);
        vTaskDelete(NULL);
    }
    return true;
}

void game1_draw()
{
    bool gameEnded = ((score >= BLOCK_COUNT) || (lives == 255));

    byte platformXtmp = platformX;

    static bool first = 1;

    // Move ball
    // hide ball
    draw_bitmap(ball.x, ball.y, clearImg, 2, 2, NOINVERT, 0);
    draw_flushArea(ball.x, ball.y, 2, 8);

    // Draw platform
    // draw_bitmap(platformX, g_yres - 8, platform, 12, 8, NOINVERT, 0);
    // draw_flushArea(platformX, g_yres - 8, 12, 8);

    if (!gameEnded) {
        ball.x += ball.velX;
        ball.y += ball.velY;
    }

    bool blockCollide = false;
    const float ballX = ball.x;
    const byte ballY  = ball.y;

    // Block collision
    byte idx = 0;
    LOOP(BLOCK_COLS, x)
    {
        LOOP(BLOCK_ROWS, y)
        {
            if (!blocks[idx] && ballX >= x * 4 && ballX < (x * 4) + 4 && ballY >= (y * 4) + 8 && ballY < (y * 4) + 8 + 4) {
                //				buzzer_buzz(100, TONE_2KHZ, VOL_UI, PRIO_UI, NULL);
                // led_flash(LED_GREEN, 50, 255); // 100ask todo
                blocks[idx] = true;

                // hide block
                draw_bitmap(x * 4, (y * 4) + 8, clearImg, 3, 8, NOINVERT, 0);
                draw_flushArea(x * 4, (y * 4) + 8, 3, 8);
                blockCollide = true;
                score++;
            }
            idx++;
        }
    }

    // Side wall collision
    if (ballX > g_xres - 2) {
        if (ballX > 240)
            ball.x = 0;
        else
            ball.x = g_xres - 2;
        ball.velX = -ball.velX;
    }
    if (ballX < 0) {
        ball.x    = 0;
        ball.velX = -ball.velX;
    }

    // Platform collision
    bool platformCollision = false;
    if (!gameEnded && ballY >= g_yres - PLATFORM_HEIGHT - 2 && ballY < 240 && ballX >= platformX && ballX <= platformX + PLATFORM_WIDTH) {
        platformCollision = true;
        // buzzer_buzz(200, TONE_5KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
        ball.y = g_yres - PLATFORM_HEIGHT - 2;
        if (ball.velY > 0)
            ball.velY = -ball.velY;
        ball.velX = ((float)rand() / (RAND_MAX / 2)) - 1; // -1.0 to 1.0
    }

    // Top/bottom wall collision
    if (!gameEnded && !platformCollision && (ballY > g_yres - 2 || blockCollide)) {
        if (ballY > 240) {
            // buzzer_buzz(200, TONE_2_5KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
            ball.y = 0;
        } else if (!blockCollide) {
            // buzzer_buzz(200, TONE_2KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
            ball.y = g_yres - 1;
            lives--;
        }
        ball.velY *= -1;
    }

    // Draw ball
    draw_bitmap(ball.x, ball.y, ballImg, 2, 2, NOINVERT, 0);
    draw_flushArea(ball.x, ball.y, 2, 8);

    // Draw platform
    // draw_bitmap(platformX, g_yres - 8, platform, 12, 8, NOINVERT, 0);
    // draw_flushArea(platformX, g_yres - 8, 12, 8);

    if (first) {
        first = 0;

        // Draw blocks
        idx = 0;
        LOOP(BLOCK_COLS, x)
        {
            LOOP(BLOCK_ROWS, y)
            {
                if (!blocks[idx]) {
                    draw_bitmap(x * 4, (y * 4) + 8, block, 3, 8, NOINVERT, 0);
                    draw_flushArea(x * 4, (y * 4) + 8, 3, 8);
                }
                idx++;
            }
        }
    }

    // Draw score
    char buff[6];
    sprintf_P(buff, PSTR("%u"), score);
    draw_string(buff, false, 0, 0);

    // Draw lives
    if (lives != 255) {
        LOOP(lives_origin, i)
        {
            if (i < lives)
                draw_bitmap((g_xres - (3 * 8)) + (8 * i), 1, livesImg, 7, 8, NOINVERT, 0);
            else
                draw_bitmap((g_xres - (3 * 8)) + (8 * i), 1, clearImg, 7, 8, NOINVERT, 0);
            draw_flushArea((g_xres - (3 * 8)) + (8 * i), 1, 7, 8);
        }
    }

    // Got all blocks
    if (score >= BLOCK_COUNT)
        draw_string_P(PSTR(STR_WIN), false, 50, 32);

    // No lives left (255 because overflow)
    if (lives == 255)
        draw_string_P(PSTR(STR_GAMEOVER), false, 34, 32);
}
