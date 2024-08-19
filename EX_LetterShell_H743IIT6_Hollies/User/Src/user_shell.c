#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "user_shell.h"
#include "shell.h"
#include "usbd_cdc_if.h"

Shell shell;
char shellBuffer[512];

uint8_t rxBuf[64] = {0};
uint8_t rxFlag = 0;

static SemaphoreHandle_t shellMutex;

/**
 * @brief 用户shell写
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{
    CDC_Transmit_FS((uint8_t *)data, len);
    return len;
}

/**
 * @brief 用户shell读
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际读取到
 */
short userShellRead(char *data, unsigned short len)
{
    if (rxFlag)
    {
        rxFlag = 0;
        memcpy(data, rxBuf, len);
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief 用户shell上锁
 *
 * @param shell shell
 *
 * @return int 0
 */
int userShellLock(Shell *shell)
{
    UNUSED(shell);
    xSemaphoreTakeRecursive(shellMutex, portMAX_DELAY);
    return 0;
}

/**
 * @brief 用户shell解锁
 *
 * @param shell shell
 *
 * @return int 0
 */
int userShellUnlock(Shell *shell)
{
    UNUSED(shell);
    xSemaphoreGiveRecursive(shellMutex);
    return 0;
}

/**
 * @brief 用户shell初始化
 *
 */
void userShellInit(void)
{
    shellMutex = xSemaphoreCreateMutex();

    shell.write = userShellWrite;
    shell.read = userShellRead;
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;
    shellInit(&shell, shellBuffer, 512);
}
