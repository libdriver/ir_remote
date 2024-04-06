/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      stm32f407_driver_ir_remote_interface.c
 * @brief     stm32f407 driver ir_remote interface source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2023-03-31
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2023/03/31  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_ir_remote_interface.h"
#include "delay.h"
#include "uart.h"
#include "tim.h"
#include <stdarg.h>

/**
 * @brief timer var definition
 */
static uint64_t gs_second = 0;        /**< second */

/**
 * @brief     timer callback
 * @param[in] us is the timer cnt
 * @note      none
 */
static void gs_tim_irq(uint32_t us)
{
    gs_second += us / 1000000;
}

/**
 * @brief  interface timer init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 * @note   none
 */
uint8_t ir_remote_interface_timer_init(void)
{
    /* timer init */
    if (tim_init(1000000, gs_tim_irq) != 0)
    {
        return 1;
    }
    
    /* timer start */
    if (tim_start() != 0)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     interface timestamp read
 * @param[in] *t points to an ir_remote_time structure
 * @return    status code
 *            - 0 success
 *            - 1 read failed
 * @note      none
 */
uint8_t ir_remote_interface_timestamp_read(ir_remote_time_t *t)
{
    t->s = gs_second;
    t->us = tim_get_handle()->Instance->CNT;
    
    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void ir_remote_interface_delay_ms(uint32_t ms)
{
    delay_ms(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void ir_remote_interface_debug_print(const char *const fmt, ...)
{
    char str[256];
    uint16_t len;
    va_list args;
    
    memset((char *)str, 0, sizeof(char) * 256); 
    va_start(args, fmt);
    vsnprintf((char *)str, 255, (char const *)fmt, args);
    va_end(args);
    
    len = strlen((char *)str);
    (void)uart_write((uint8_t *)str, len);
}

/**
 * @brief     interface receive callback
 * @param[in] *data points to an ir_remote_t structure
 * @note      none
 */
void ir_remote_interface_receive_callback(ir_remote_t *data)
{
    switch (data->status)
    {
        case IR_REMOTE_STATUS_OK :
        {
            ir_remote_interface_debug_print("ir_remote: irq ok.\n");
            ir_remote_interface_debug_print("ir_remote: add is 0x%02X and cmd is 0x%02X.\n", data->address, data->command);
            
            break;
        }
        case IR_REMOTE_STATUS_REPEAT :
        {
            ir_remote_interface_debug_print("ir_remote: irq repeat.\n");
            ir_remote_interface_debug_print("ir_remote: add is 0x%02X and cmd is 0x%02X.\n", data->address, data->command);
            
            break;
        }
        case IR_REMOTE_STATUS_ADDR_ERR :
        {
            ir_remote_interface_debug_print("ir_remote: irq addr error.\n");
            
            break;
        }
        case IR_REMOTE_STATUS_CMD_ERR :
        {
            ir_remote_interface_debug_print("ir_remote: irq cmd error.\n");
            
            break;
        }
        case IR_REMOTE_STATUS_FRAME_INVALID :
        {
            ir_remote_interface_debug_print("ir_remote: irq frame invalid.\n");
            
            break;
        }
        default :
        {
            ir_remote_interface_debug_print("ir_remote: irq unknown status.\n");
            
            break;
        }
    }
}
