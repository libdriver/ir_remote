/**
 * Copyright (c) 2022 - present LibTutorial All rights reserved
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
 * @file      tim.h
 * @brief     tim header file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2022-11-11
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2022/11/11  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#ifndef TIM_H
#define TIM_H

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @defgroup tim tim function
 * @brief    tim function modules
 * @{
 */

/**
 * @brief     timer init
 * @param[in] us is the interrupt time
 * @param[in] *tim_irq points to a timer irq function address
 * @return    status code
 *            - 0 success
 *            - 1 init failed
 *            - 2 us must be over zero
 * @note      none
 */
uint8_t tim_init(uint32_t us, void (*tim_irq)(uint32_t us));

/**
 * @brief  timer deint
 * @return status code
 *         - 0 success
 *         - 1 deinit
 * @note   none
 */
uint8_t tim_deinit(void);

/**
 * @brief  start the timer
 * @return status code
 *         - 0 success
 *         - 1 start failed
 * @note   none
 */
uint8_t tim_start(void);

/**
 * @brief  stop the timer
 * @return status code
 *         - 0 success
 *         - 1 stop failed
 * @note   none
 */
uint8_t tim_stop(void);

/**
 * @brief  get the timer handle
 * @return points to a timer handle
 * @note   none
 */
TIM_HandleTypeDef* tim_get_handle(void);

/**
 * @brief timer irq handler
 * @note  none
 */
void tim_irq_handler(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
