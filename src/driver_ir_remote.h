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
 * @file      driver_ir_remote.h
 * @brief     driver ir remote header file
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

#ifndef DRIVER_IR_REMOTE_H
#define DRIVER_IR_REMOTE_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup ir_remote_driver ir_remote driver function
 * @brief    ir_remote driver modules
 * @{
 */

/**
 * @addtogroup ir_remote_basic_driver
 * @{
 */

/**
 * @brief ir_remote max range definition
 */
#ifndef IR_REMOTE_MAX_RANGE
    #define IR_REMOTE_MAX_RANGE        0.20f        /**< 20% */
#endif

/**
 * @brief ir_remote status enumeration definition
 */
typedef enum
{
    IR_REMOTE_STATUS_OK            = 0x00,        /**< ok */
    IR_REMOTE_STATUS_REPEAT        = 0x01,        /**< repeat */
    IR_REMOTE_STATUS_ADDR_ERR      = 0x02,        /**< addr error */
    IR_REMOTE_STATUS_CMD_ERR       = 0x03,        /**< cmd error */
    IR_REMOTE_STATUS_FRAME_INVALID = 0x04,        /**< frame invalid */
} ir_remote_status_t;

/**
 * @brief ir_remote structure definition
 */
typedef struct ir_remote_s
{
    uint8_t status;        /**< status */
    uint8_t address;       /**< address */
    uint8_t command;       /**< command */
} ir_remote_t;

/**
 * @brief ir_remote time structure definition
 */
typedef struct ir_remote_time_s
{
    uint64_t s;         /**< second */
    uint32_t us;        /**< microsecond */
} ir_remote_time_t;

/**
 * @brief ir_remote decode structure definition
 */
typedef struct ir_remote_decode_s
{
    ir_remote_time_t t;        /**< timestamp */
    uint32_t diff_us;          /**< diff us */
} ir_remote_decode_t;

/**
 * @brief ir_remote handle structure definition
 */
typedef struct ir_remote_handle_s
{
    uint8_t (*timestamp_read)(ir_remote_time_t *t);         /**< point to an timestamp_read function address */
    void (*delay_ms)(uint32_t ms);                          /**< point to a delay_ms function address */
    void (*debug_print)(const char *const fmt, ...);        /**< point to a debug_print function address */
    void (*receive_callback)(ir_remote_t *data);            /**< point to a receive_callback function address */
    uint8_t inited;                                         /**< inited flag */
    ir_remote_decode_t decode[128];                         /**< decode buffer */
    uint16_t decode_len;                                    /**< decode length */
    ir_remote_time_t last_time;                             /**< last time */
    ir_remote_t last_code;                                  /**< last code */
} ir_remote_handle_t;

/**
 * @brief ir_remote information structure definition
 */
typedef struct ir_remote_info_s
{
    char chip_name[32];                /**< chip name */
    char manufacturer_name[32];        /**< manufacturer name */
    char interface[8];                 /**< chip interface name */
    float supply_voltage_min_v;        /**< chip min supply voltage */
    float supply_voltage_max_v;        /**< chip max supply voltage */
    float max_current_ma;              /**< chip max current */
    float temperature_min;             /**< chip min operating temperature */
    float temperature_max;             /**< chip max operating temperature */
    uint32_t driver_version;           /**< driver version */
} ir_remote_info_t;

/**
 * @}
 */

/**
 * @defgroup ir_remote_link_driver ir_remote link driver function
 * @brief    ir_remote link driver modules
 * @ingroup  ir_remote_driver
 * @{
 */

/**
 * @brief     initialize ir_remote_handle_t structure
 * @param[in] HANDLE points to an ir_remote handle structure
 * @param[in] STRUCTURE is ir_remote_handle_t
 * @note      none
 */
#define DRIVER_IR_REMOTE_LINK_INIT(HANDLE, STRUCTURE)         memset(HANDLE, 0, sizeof(STRUCTURE))

/**
 * @brief     link timestamp_read function
 * @param[in] HANDLE points to an ir_remote handle structure
 * @param[in] FUC points to a timestamp_read function address
 * @note      none
 */
#define DRIVER_IR_REMOTE_LINK_TIMESTAMP_READ(HANDLE, FUC)    (HANDLE)->timestamp_read = FUC

/**
 * @brief     link delay_ms function
 * @param[in] HANDLE points to an ir_remote handle structure
 * @param[in] FUC points to a delay_ms function address
 * @note      none
 */
#define DRIVER_IR_REMOTE_LINK_DELAY_MS(HANDLE, FUC)          (HANDLE)->delay_ms = FUC

/**
 * @brief     link debug_print function
 * @param[in] HANDLE points to an ir_remote handle structure
 * @param[in] FUC points to a debug_print function address
 * @note      none
 */
#define DRIVER_IR_REMOTE_LINK_DEBUG_PRINT(HANDLE, FUC)       (HANDLE)->debug_print = FUC

/**
 * @brief     link receive_callback function
 * @param[in] HANDLE points to an ir_remote handle structure
 * @param[in] FUC points to a receive_callback function address
 * @note      none
 */
#define DRIVER_IR_REMOTE_LINK_RECEIVE_CALLBACK(HANDLE, FUC)  (HANDLE)->receive_callback = FUC

/**
 * @}
 */

/**
 * @defgroup ir_remote_basic_driver ir_remote basic driver function
 * @brief    ir_remote basic driver modules
 * @ingroup  ir_remote_driver
 * @{
 */

/**
 * @brief      get chip's information
 * @param[out] *info points to an ir_remote info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ir_remote_info(ir_remote_info_t *info);

/**
 * @brief     irq handler
 * @param[in] *handle points to an ir_remote handle structure
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ir_remote_irq_handler(ir_remote_handle_t *handle);

/**
 * @brief     initialize the chip
 * @param[in] *handle points to an ir_remote handle structure
 * @return    status code
 *            - 0 success
 *            - 1 gpio initialization failed
 *            - 2 handle is NULL
 *            - 3 linked functions is NULL
 * @note      none
 */
uint8_t ir_remote_init(ir_remote_handle_t *handle);

/**
 * @brief     close the chip
 * @param[in] *handle points to an ir_remote handle structure
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ir_remote_deinit(ir_remote_handle_t *handle);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
