#ifndef __UART_DEBUG__
#define __UART_DEBUG__

#define UART_DEBUG_EN 1 // 使能/除能 模拟串口调试模块

#if UART_DEBUG_EN == 1

#define PIN_UART_DEBUG        P3_2 // IO口定义
#define PIN_UART_DEBUG_OUT    P3CR |= BIT2 // IO口设置为输出

#define uartState_Idle     0
#define uartState_Start    1
#define uartState_Data     2
#define uartState_Verify   3
#define uartState_Stop     4

extern void uart_debug(void);


#define UART_VERIFY_BIT    0 // 使能/除能 奇偶校验位


#else

#define uart_debug()   
#define PIN_UART_DEBUG_OUT  

#endif




#endif

