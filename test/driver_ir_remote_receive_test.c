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
 * @file      driver_ir_remote_receive_test.c
 * @brief     driver ir_remote receive test source file
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

#include "driver_ir_remote_receive_test.h"

static ir_remote_handle_t gs_handle;        /**< ir_remote handle */
static volatile uint8_t gs_flag;            /**< flag */

/**
 * @brief  receive test irq
 * @return status code
 *         - 0 success
 *         - 1 run failed
 * @note   none
 */
uint8_t ir_remote_receive_test_irq_handler(void)
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
 * @brief     interface receive callback
 * @param[in] *data points to an ir_remote_t structure
 * @note      none
 */
static void a_receive_callback(ir_remote_t *data)
{
    switch (data->status)
    {
        case IR_REMOTE_STATUS_OK :
        {
            ir_remote_interface_debug_print("ir_remote: irq ok.\n");
            ir_remote_interface_debug_print("ir_remote: add is 0x%02X and cmd is 0x%02X.\n", data->address, data->command);
            gs_flag = 1;
            
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

/**
 * @brief     receive test
 * @param[in] times is the test times
 * @return    status code
 *            - 0 success
 *            - 1 test failed
 * @note      none
 */
uint8_t ir_remote_receive_test(uint32_t times)
{
    uint8_t res;
    uint16_t timeout;
    uint32_t i;
    ir_remote_info_t info;
    
    /* link interface function */
    DRIVER_IR_REMOTE_LINK_INIT(&gs_handle, ir_remote_handle_t);
    DRIVER_IR_REMOTE_LINK_TIMESTAMP_READ(&gs_handle, ir_remote_interface_timestamp_read);
    DRIVER_IR_REMOTE_LINK_DELAY_MS(&gs_handle, ir_remote_interface_delay_ms);
    DRIVER_IR_REMOTE_LINK_DEBUG_PRINT(&gs_handle, ir_remote_interface_debug_print);
    DRIVER_IR_REMOTE_LINK_RECEIVE_CALLBACK(&gs_handle, a_receive_callback);
    
    /* get information */
    res = ir_remote_info(&info);
    if (res != 0)
    {
        ir_remote_interface_debug_print("ir_remote: get info failed.\n");
       
        return 1;
    }
    else
    {
        /* print chip info */
        ir_remote_interface_debug_print("ir_remote: chip is %s.\n", info.chip_name);
        ir_remote_interface_debug_print("ir_remote: manufacturer is %s.\n", info.manufacturer_name);
        ir_remote_interface_debug_print("ir_remote: interface is %s.\n", info.interface);
        ir_remote_interface_debug_print("ir_remote: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        ir_remote_interface_debug_print("ir_remote: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        ir_remote_interface_debug_print("ir_remote: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        ir_remote_interface_debug_print("ir_remote: max current is %0.2fmA.\n", info.max_current_ma);
        ir_remote_interface_debug_print("ir_remote: max temperature is %0.1fC.\n", info.temperature_max);
        ir_remote_interface_debug_print("ir_remote: min temperature is %0.1fC.\n", info.temperature_min);
    }
    
    /* init */
    res = ir_remote_init(&gs_handle);
    if (res != 0)
    {
        ir_remote_interface_debug_print("ir_remote: init failed.\n");
       
        return 1;
    }
    
    /* start receive test */
    ir_remote_interface_debug_print("ir_remote: start receive test.\n");
    
    /* loop */
    for (i = 0; i < times; i++)
    {
        /* 5s timeout */
        timeout = 500;
        
        /* init 0 */
        gs_flag = 0;
        
        /* check timeout */
        while (timeout != 0)
        {
            /* check the flag */
            if (gs_flag != 0)
            {
                break;
            }
            
            /* timeout -- */
            timeout--;
            
            /* delay 10ms */
            ir_remote_interface_delay_ms(10);
        }
        
        /* check the timeout */
        if (timeout == 0)
        {
            /* receive timeout */
            ir_remote_interface_debug_print("ir_remote: receive timeout.\n");
            (void)ir_remote_deinit(&gs_handle);
                
            return 1;
        }
    }
    
    /* finish receive test */
    ir_remote_interface_debug_print("ir_remote: finish receive test.\n");
    (void)ir_remote_deinit(&gs_handle);
    
    return 0;
}
