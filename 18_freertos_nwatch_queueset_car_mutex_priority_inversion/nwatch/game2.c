#include "typedefs.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "draw.h"

#define CAR_COUNT  (3u)
#define CAR_WIDTH  (12u)
#define CAR_LENGTH (15u)
#define ROAD_SPEED (6u)

#define NOINVERT   false
#define INVERT     true

static uint32_t g_xres, g_yres, g_bpp;
static uint8_t *g_framebuffer;
static SemaphoreHandle_t g_xSemOfTicksHandle;

static const byte carImg[] PROGMEM = {
    0x40,
    0xF8,
    0xEC,
    0x2C,
    0x2C,
    0x38,
    0xF0,
    0x10,
    0xD0,
    0x30,
    0xE8,
    0x4C,
    0x4C,
    0x9C,
    0xF0,
    0x02,
    0x1F,
    0x37,
    0x34,
    0x34,
    0x1C,
    0x0F,
    0x08,
    0x0B,
    0x0C,
    0x17,
    0x32,
    0x32,
    0X39,
    0X0F,
};

static const byte roadMarking[] PROGMEM = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};

static const byte clearImg[sizeof(carImg)] = {0};

typedef struct
{
    int x;
    int y;
    int control_key;
} CarTaskParm;

CarTaskParm g_cars[3] = {
    {0, 0, NUM1},
    {0, 17, NUM2},
    {0, 34, NUM3},
};

#if 0
void car_test(void)
{
    g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
    draw_init();
    draw_end();

    draw_bitmap(0, 0, carImg, 15, 16, NOINVERT, 0);
    draw_flushArea(0, 0, 15, 16);

    draw_bitmap(0, 16, roadMarking, 8, 1, NOINVERT, 0);
    draw_flushArea(0, 16, 8, 1);
    while (1) {
        /* code */
    }
}
#endif // 0
static void ShowCar(CarTaskParm *pCarTaskparm)
{
    draw_bitmap(pCarTaskparm->x, pCarTaskparm->y, carImg, 15, 16, NOINVERT, 0);
    draw_flushArea(pCarTaskparm->x, pCarTaskparm->y, 15, 16);
}

static void HideCar(CarTaskParm *pCarTaskparm)
{
    draw_bitmap(pCarTaskparm->x, pCarTaskparm->y, clearImg, 15, 16, NOINVERT, 0);
    draw_flushArea(pCarTaskparm->x, pCarTaskparm->y, 15, 16);
}

static void Car1Task(void *parm)
{
    CarTaskParm *pCarTaskParm = (CarTaskParm *)parm;
    IrRecvData irRecvDdata;
    ShowCar(pCarTaskParm);
    // 创建自己的队列
    QueueHandle_t xQueueIRHandle;
    xQueueIRHandle = xQueueCreate(10, sizeof(IrRecvData)); // 主要是利用任务函数拥有自己独立的栈，写申请一个队列即可达到3个队列的效果，在放入队列数组中，在驱动函数里分发

    RegisterQueueHandle(xQueueIRHandle);

    // 获得信号量
    xSemaphoreTake(g_xSemOfTicksHandle, portMAX_DELAY);
    for (;;) {
        /* 读取IR值 */
        // xQueueReceive(xQueueIRHandle, &irRecvDdata, portMAX_DELAY);

        /* 控制汽车往右移动 */
        // if (irRecvDdata.val == pCarTaskParm->control_key)
        {
            if (pCarTaskParm->x < g_xres - CAR_LENGTH) {
                /* 隐藏汽车 */
                HideCar(pCarTaskParm);
                /* 调整位置 */
                pCarTaskParm->x += 1;
                if (pCarTaskParm->x > g_xres - CAR_LENGTH) {
                    pCarTaskParm->x = g_xres - CAR_LENGTH;
                }
                /* 重新绘制 */
                ShowCar(pCarTaskParm);

                vTaskDelay(50);

                if (pCarTaskParm->x == g_xres - CAR_LENGTH) {
                    xSemaphoreGive(g_xSemOfTicksHandle);
                    vTaskDelete(NULL);
                }
            }
        }
    }
}

static void Car2Task(void *parm)
{
    CarTaskParm *pCarTaskParm = (CarTaskParm *)parm;
    IrRecvData irRecvDdata;
    ShowCar(pCarTaskParm);
    // 创建自己的队列
    QueueHandle_t xQueueIRHandle;
    xQueueIRHandle = xQueueCreate(10, sizeof(IrRecvData)); // 主要是利用任务函数拥有自己独立的栈，写申请一个队列即可达到3个队列的效果，在放入队列数组中，在驱动函数里分发

    RegisterQueueHandle(xQueueIRHandle);

    vTaskDelay(1000);
    for (;;) {
        /* 读取IR值 */
        // xQueueReceive(xQueueIRHandle, &irRecvDdata, portMAX_DELAY);

        /* 控制汽车往右移动 */
        // if (irRecvDdata.val == pCarTaskParm->control_key)
        {
            if (pCarTaskParm->x < g_xres - CAR_LENGTH) {
                /* 隐藏汽车 */
                HideCar(pCarTaskParm);
                /* 调整位置 */
                pCarTaskParm->x += 1;
                if (pCarTaskParm->x > g_xres - CAR_LENGTH) {
                    pCarTaskParm->x = g_xres - CAR_LENGTH;
                }
                /* 重新绘制 */
                ShowCar(pCarTaskParm);

                mdelay(50);

                if (pCarTaskParm->x == g_xres - CAR_LENGTH) {
                    vTaskDelete(NULL);
                }
            }
        }
    }
}

static void Car3Task(void *parm)
{
    CarTaskParm *pCarTaskParm = (CarTaskParm *)parm;
    IrRecvData irRecvDdata;
    ShowCar(pCarTaskParm);
    vTaskDelay(2000);
    // 创建自己的队列
    QueueHandle_t xQueueIRHandle;
    xQueueIRHandle = xQueueCreate(10, sizeof(IrRecvData)); // 主要是利用任务函数拥有自己独立的栈，写申请一个队列即可达到3个队列的效果，在放入队列数组中，在驱动函数里分发

    RegisterQueueHandle(xQueueIRHandle);

    // 获得信号量
    xSemaphoreTake(g_xSemOfTicksHandle, portMAX_DELAY);
    for (;;) {
        /* 读取IR值 */
        // xQueueReceive(xQueueIRHandle, &irRecvDdata, portMAX_DELAY);

        /* 控制汽车往右移动 */
        // if (irRecvDdata.val == pCarTaskParm->control_key)
        {
            if (pCarTaskParm->x < g_xres - CAR_LENGTH) {
                /* 隐藏汽车 */
                HideCar(pCarTaskParm);
                /* 调整位置 */
                pCarTaskParm->x += 1;
                if (pCarTaskParm->x > g_xres - CAR_LENGTH) {
                    pCarTaskParm->x = g_xres - CAR_LENGTH;
                }
                /* 重新绘制 */
                ShowCar(pCarTaskParm);

                vTaskDelay(50);

                if (pCarTaskParm->x == g_xres - CAR_LENGTH) {
                    xSemaphoreGive(g_xSemOfTicksHandle);
                    vTaskDelete(NULL);
                }
            }
        }
    }
}

void car_game(void *argument)
{
    g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
    draw_init();
    draw_end();

    /* 绘制路标 */
    for (size_t i = 0; i < CAR_COUNT; i++) {
        for (size_t j = 0; j < 8; j++) {                                      // = 128/8/2
            draw_bitmap(16 * j, 16 + 17 * i, roadMarking, 8, 1, NOINVERT, 0); // 从像素点去理解
            draw_flushArea(16 * j, 16 + 17 * i, 8, 1);
        }
    }

    g_xSemOfTicksHandle = xSemaphoreCreateMutex();

#if 0
    /* 绘制汽车 */
    for (size_t i = 0; i < CAR_COUNT; i++) {
        draw_bitmap(g_cars[i].x, g_cars[i].y, carImg, 15, 16, NOINVERT, 0);
        draw_flushArea(g_cars[i].x, g_cars[i].y, 15, 16);
    }
#endif

    /* 创建三个汽车任务 */
    xTaskCreate(Car1Task, "Car1Task", 128, &g_cars[0], osPriorityNormal, NULL);
    xTaskCreate(Car2Task, "Car2Task", 128, &g_cars[1], osPriorityNormal1, NULL);
    xTaskCreate(Car3Task, "Car3Task", 128, &g_cars[2], osPriorityNormal2, NULL);

    vTaskDelete(NULL);
}