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
 * @file      tim.c
 * @brief     tim source file
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

#include "tim.h"

/**
 * @brief tim var definition
 */
static TIM_HandleTypeDef gs_tim_handle;               /**< tim handle */
static void (*gs_tim_irq)(uint32_t us) = NULL;        /**< tim irq */

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
uint8_t tim_init(uint32_t us, void (*tim_irq)(uint32_t us))
{
    /* check the us and us must be over zero */
    if (us == 0)
    {
        return 2;
    }
    
    /* use TIM2 */
    gs_tim_handle.Instance = TIM2;
    
    /* set the prescaler */
    gs_tim_handle.Init.Prescaler = ((SystemCoreClock / 2) / 1000000) - 1;
    
    /* up counter mode */
    gs_tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    
    /* set us */
    gs_tim_handle.Init.Period = us - 1;
    
    /* div 1 */
    gs_tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    
    /* time base init */
    if (HAL_TIM_Base_Init(&gs_tim_handle) != HAL_OK)
    {
        return 1;
    }
    
    /* set the timer callback */
    gs_tim_irq = tim_irq;
    
    return 0;
}

/**
 * @brief  timer deint
 * @return status code
 *         - 0 success
 *         - 1 deinit
 * @note   none
 */
uint8_t tim_deinit(void)
{
    /* timer deinit */
    if (HAL_TIM_Base_DeInit(&gs_tim_handle) != HAL_OK)
    {
        return 1;
    }
    
    /* set tim irq NULL */
    gs_tim_irq = NULL;
    
    return 0;
}

/**
 * @brief  start the timer
 * @return status code
 *         - 0 success
 *         - 1 start failed
 * @note   none
 */
uint8_t tim_start(void)
{
    /* start the timer */
    if (HAL_TIM_Base_Start_IT(&gs_tim_handle) != HAL_OK)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  stop the timer
 * @return status code
 *         - 0 success
 *         - 1 stop failed
 * @note   none
 */
uint8_t tim_stop(void)
{
    /* stop the timer */
    if (HAL_TIM_Base_Stop_IT(&gs_tim_handle) != HAL_OK)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief  get the timer handle
 * @return points to a timer handle
 * @note   none
 */
TIM_HandleTypeDef* tim_get_handle(void)
{
    return &gs_tim_handle;
}

/**
 * @brief timer irq handler
 * @note  none
 */
void tim_irq_handler(void)
{
    /* if tim irq not NULL */
    if (gs_tim_irq != NULL)
    {
        /* run the tim irq */
        gs_tim_irq(gs_tim_handle.Instance->ARR + 1);
    }
}
