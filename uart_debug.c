#include "uart_debug.h"

#if UART_DEBUG_EN == 1


static uint8_t uartState; // 串口发送阶段

static uint8_t uart_debug_buf[10]; // 发送数据, 前两位为帧头, 最后为校验和
static uint8_t i, j; // 发送索引/位
static uint16_t uart_debug_cnt; // 帧发送间隔

#if UART_VERIFY_BIT == 1
static uint8_t verify_cnt; // 奇偶校验计数
#endif

static uint8_t Sum_Make_Verify (uint8_t *PBuf, uint8_t Len) // 校验和计算
{
    uint8_t Verify = 0;
    while (Len--)
    {
        Verify = Verify + (*PBuf);
        ++ PBuf;
    }
    return (~Verify + 1);
}


void uart_debug(void) // 每调用一次, 发送一位串口信号, 波特率根据调用的时间间隔计算, 例如: 1ms调用一次, 波特率为 1000
{
    switch(uartState) // 串口发送阶段
    {
    case(uartState_Idle): // 发送前数据准备
    {
        PIN_UART_DEBUG = 1;

        if(uart_debug_cnt > 0)
        {
            uart_debug_cnt--;
        }
        else
        {
            uart_debug_cnt = 400; // send interval

            uart_debug_buf[0] = 0xA5; // Header
            uart_debug_buf[1] = 0x5A; // Header


            uart_debug_buf[2] = 0x34; // data
            uart_debug_buf[3] = 0x56;
            uart_debug_buf[4] = 0x78;
            uart_debug_buf[5] = 0x9A;
            uart_debug_buf[6] = 0xBC;
            uart_debug_buf[7] = 0xDE;
            uart_debug_buf[8] = 0xF0;

            uart_debug_buf[9] = Sum_Make_Verify(uart_debug_buf, 9); // Check Sum

            uartState = uartState_Start;
        }
        break;
    }
    case(uartState_Start): // 起始位
    {
        PIN_UART_DEBUG = 0;
        uartState = uartState_Data;
        break;
    }
    case(uartState_Data): // 数据位
    {

        if((uart_debug_buf[j]) & (0x01 << i))
        {
#if UART_VERIFY_BIT == 1
            verify_cnt++;
#endif
            PIN_UART_DEBUG = 1;
        }
        else
        {
            i = i;
            PIN_UART_DEBUG = 0;
        }

        if(i >= 7)
        {
            i = 0;
            j++;

#if UART_VERIFY_BIT == 1
            uartState = uartState_Verify; // Has Verify
#else
            uartState = uartState_Stop; // No Verify
#endif
        }
        else
        {
            i++;
        }

        break;
    }
#if UART_VERIFY_BIT == 1
    case(uartState_Verify): // 奇偶校验位
    {

        PIN_UART_DEBUG = (verify_cnt % 2); // EVEN Verify

        verify_cnt = 0;

        uartState = uartState_Stop;
        break;
    }
#endif
    case(uartState_Stop): // 停止位
    {
        PIN_UART_DEBUG = 1;

        if(j >= 10)
        {
            i = 0;
            j = 0;
            uartState = uartState_Idle;
        }
        else
        {
            uartState = uartState_Start;
        }

        break;
    }
    default:
    {
        PIN_UART_DEBUG = 1;
        uartState = uartState_Idle;
        break;
    }
    }

}

#endif
