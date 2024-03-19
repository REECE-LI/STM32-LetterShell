#include "selfmain.h"
#include "main.h"
#include "Retarget.h"
#include "usart.h"
#include "shell.h"




void SelfMain() {

    RetargetInit(&huart1);

    shell.write = ShellWrite;

    shellInit(&shell);

    HAL_UART_Receive_IT(&huart1, (uint8_t *)&shellRxBuffer, 1);

    for (;;) {
        HAL_Delay(1);
    }
}

// 串口中断
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        shellInput(&shell, shellRxBuffer);
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&shellRxBuffer, 1);
    }
}