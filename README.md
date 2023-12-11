## STM32开发环境说明：

### 使用工具：

#### MCU：STM32F103C8T6

​          Arm Cortex-M3 MCU with 64 Kbytes of  Flash memory, 72 MHz CPU

#### CubeMX：V6.9.2

#### VSCode：安装EIDE插件 其中集成了编译、下载、调试等额外开发插件

#### 交叉编译工具链：(GNU Arm Embedded Toolchain 10-2020-q4-major) 10.2.1 20201103 (release)

#### 下载：STM32CubeProgrammer v2.8.0 

#### 调试：openocd

### 具体使用步骤说明：

#### 开发流程说明：http://t.csdnimg.cn/COQKi

#### EIDE使用论坛：[Embedded IDE Forum (em-ide.com)](https://discuss.em-ide.com/)



## 项目工程文件夹说明

### 01_freertos_template

一个使用CubeMX初始化STM32F103C8T6相关外设的模板工程，里面包含了相关外设驱动的代码

### 02_freertos_create_the_first_multitask

使用CubeMX通过CMSIS接口开启多任务

### 03_freertos_create_multitask

使用xTaskCreate/xTaskCreateStatic手动创建多任务

```C
BaseType_t xTaskCreate ( TaskFunction_t pxTaskCode, // 函数指针, 任务函数
                        const char * const pcName, // 任务的名字
                        const configSTACK_DEPTH_TYPE usStackDepth, // 栈大小,单位为word,10表示40字节
                        void * const pvParameters, // 调用任务函数时传入的参数
                        UBaseType_t uxPriority,    // 优先级
                        TaskHandle_t * const pxCreatedTask ); // 任务句柄, 以后使用它来操作这个任务
```

|   **参数**    | **描述**                                                     |
| :-----------: | :----------------------------------------------------------- |
|  pvTaskCode   | 函数指针，任务对应的 C 函数。任务应该永远不退出，或者在退出时调用 "vTaskDelete(NULL)"。 |
|    pcName     | 任务的名称，仅用于调试目的，FreeRTOS 内部不使用。pcName 的长度为 configMAX_TASK_NAME_LEN |
| usStackDepth  | 每个任务都有自己的栈，usStackDepth 指定了栈的大小，单位为 word。例如，如果传入 100，表示栈的大小为 100 word，即 400 字节。最大值为 uint16_t 的最大值。确定栈的大小并不容易，通常是根据估计来设定。精确的办法是查看反汇编代码。 |
| pvParameters  | 调用 pvTaskCode 函数指针时使用的参数：pvTaskCode(pvParameters)。 |
|  uxPriority   | 任务的优先级范围为 0~(configMAX_PRIORITIES – 1)。数值越小，优先级越低。如果传入的值过大，xTaskCreate 会将其调整为 (configMAX_PRIORITIES – 1)。 |
| pxCreatedTask | 用于保存 xTaskCreate 的输出结果，即任务的句柄（task handle）。如果以后需要对该任务进行操作，如修改优先级，则需要使用此句柄。如果不需要使用该句柄，可以传入 NULL。 |

```c
TaskHandle_t xTaskCreateStatic ( TaskFunction_t pxTaskCode,   // 函数指针, 任务函数
                                 const char * const pcName,   // 任务的名字
                                 const uint32_t ulStackDepth, // 栈大小,单位为word,10表示40字节
                                 void * const pvParameters,   // 调用任务函数时传入的参数
                                 UBaseType_t uxPriority,      // 优先级
                                 StackType_t * const puxStackBuffer, // 静态分配的栈，就是一个buffer
                                 StaticTask_t * const pxTaskBuffer );// 静态分配的任务结构体的指针，用它来操作这个任务
```

### 04_freertos_create_multitask_with_para

使用结构体传递参数

### 05_freertos_delete_task

使用vTaskDelete删除任务

```c
void vTaskDelete ( TaskHandle_t xTaskToDelete );
NULL：
```



### 06_freertos_task_priority



### 07_freertos_task_suspend



### 08_freertos_idle_task