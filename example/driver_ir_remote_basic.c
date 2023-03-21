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
 * @file      driver_ir_remote_basic.c
 * @brief     driver ir_remote basic source file
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

#include "driver_ir_remote_basic.h"

static ir_remote_handle_t gs_handle;        /**< ir_remote handle */

/**
 * @brief  basic irq
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t ir_remote_basic_irq_handler(void)
{
    if (ir_remote_irq_handler(&gs_handle) != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief     basic example init
 * @param[in] *callback points to an irq callback address
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 * @note      none
 */
uint8_t ir_remote_basic_init(void (*callback)(ir_remote_t *data))
{
    uint8_t res;
    
    /* link interface function */
    DRIVER_IR_REMOTE_LINK_INIT(&gs_handle, ir_remote_handle_t);
    DRIVER_IR_REMOTE_LINK_TIMESTAMP_READ(&gs_handle, ir_remote_interface_timestamp_read);
    DRIVER_IR_REMOTE_LINK_DELAY_MS(&gs_handle, ir_remote_interface_delay_ms);
    DRIVER_IR_REMOTE_LINK_DEBUG_PRINT(&gs_handle, ir_remote_interface_debug_print);
    DRIVER_IR_REMOTE_LINK_RECEIVE_CALLBACK(&gs_handle, callback);
    
    /* init */
    res = ir_remote_init(&gs_handle);
    if (res != 0)
    {
        ir_remote_interface_debug_print("ir_remote: init failed.\n");
       
        return 1;
    }
    
    return 0;
}

/**
 * @brief  basic example deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t ir_remote_basic_deinit(void)
{
    if (ir_remote_deinit(&gs_handle) != 0)
    {
        return 1;
    }
    
    return 0;
}
