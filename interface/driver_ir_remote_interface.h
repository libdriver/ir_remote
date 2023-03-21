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
 * @file      driver_ir_remote_interface.h
 * @brief     driver ir_remote interface header file
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

#ifndef DRIVER_IR_REMOTE_INTERFACE_H
#define DRIVER_IR_REMOTE_INTERFACE_H

#include "driver_ir_remote.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup ir_remote_interface_driver ir_remote interface driver function
 * @brief    ir_remote interface driver modules
 * @ingroup  ir_remote_driver
 * @{
 */

/**
 * @brief     interface timestamp read
 * @param[in] *t points to an ir_remote_time structure
 * @return    status code
 *            - 0 success
 *            - 1 read failed
 * @note      none
 */
uint8_t ir_remote_interface_timestamp_read(ir_remote_time_t *t);

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void ir_remote_interface_delay_ms(uint32_t ms);

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void ir_remote_interface_debug_print(const char *const fmt, ...);

/**
 * @brief     interface receive callback
 * @param[in] *data points to an ir_remote_t structure
 * @note      none
 */
void ir_remote_interface_receive_callback(ir_remote_t *data);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
