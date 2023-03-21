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
 * @file      driver_ir_remote.c
 * @brief     driver ir remote source file
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

#include "driver_ir_remote.h"
#include <stdlib.h>

/**
 * @brief chip information definition
 */
#define CHIP_NAME                 "NEC IR REMOTE"        /**< chip name */
#define MANUFACTURER_NAME         "NEC"                  /**< manufacturer name */
#define SUPPLY_VOLTAGE_MIN        2.7f                   /**< chip min supply voltage */
#define SUPPLY_VOLTAGE_MAX        5.5f                   /**< chip max supply voltage */
#define MAX_CURRENT               1.5f                   /**< chip max current */
#define TEMPERATURE_MIN           -40.0f                 /**< chip min operating temperature */
#define TEMPERATURE_MAX           125.0f                 /**< chip max operating temperature */
#define DRIVER_VERSION            1000                   /**< driver version */

/**
 * @brief frame check definition
 */
#define IR_REMOTE_CHECK_START_HIGH        9000        /**< start frame high time */
#define IR_REMOTE_CHECK_START_LOW         4500        /**< start frame low time */
#define IR_REMOTE_CHECK_DATA0_HIGH        560         /**< bit 0 frame high time */
#define IR_REMOTE_CHECK_DATA0_LOW         560         /**< bit 0 frame low time */
#define IR_REMOTE_CHECK_DATA1_HIGH        560         /**< bit 1 frame high time */
#define IR_REMOTE_CHECK_DATA1_LOW         1680        /**< bit 1 frame low time */
#define IR_REMOTE_CHECK_DATA_0_1_EDGE     1000        /**< data 0 and 1 edge */
#define IR_REMOTE_CHECK_STOP              560         /**< stop time */
#define IR_REMOTE_CHECK_REPEAT            2250        /**< repeat time */

/**
 * @brief     check the frame time
 * @param[in] check is the checked time
 * @param[in] t is the standard time
 * @return    status code
 *            - 0 success
 *            - 1 checked failed
 * @note      none
 */
static inline uint8_t a_check_frame(uint16_t check, uint16_t t)
{
    if (abs(check - t) > (int)((float)(t) * IR_REMOTE_MAX_RANGE))        /* check the time */
    {
        return 1;                                                        /* check failed */
    }
    else
    {
        return 0;                                                        /* success return 0 */
    }
}

/**
 * @brief     ir_remote nec repeat decode
 * @param[in] *handle points to an ir_remote handle structure
 * @note      none
 */
static void a_ir_remote_nec_repeat_decode(ir_remote_handle_t *handle)
{
    uint16_t i;
    uint16_t len;
    ir_remote_t data;
    
    len = handle->decode_len - 1;                                                         /* len - 1 */
    for (i = 0; i < len; i++)                                                             /* diff all time */
    {
        int64_t diff;
        
        diff = (int64_t)((int64_t)handle->decode[i + 1].t.s -
               (int64_t)handle->decode[i].t.s) * 1000000 + 
               (int64_t)((int64_t)handle->decode[i + 1].t.us -
               (int64_t)handle->decode[i].t.us);                                          /* diff time */
        handle->decode[i].diff_us = (uint32_t)diff;                                       /* save the time diff */
    }
    
    if (a_check_frame(handle->decode[0].diff_us, IR_REMOTE_CHECK_START_HIGH) != 0)        /* check start diff */
    {
        if (handle->receive_callback != NULL)                                             /* check the receive callback */
        {
            data.address = 0x00;                                                          /* set address 0x00 */
            data.command = 0x00;                                                          /* set command 0x00 */
            data.status = IR_REMOTE_STATUS_FRAME_INVALID;                                 /* frame invalid */
            handle->receive_callback(&data);                                              /* run the callback */
        }
        
        return;                                                                           /* return */
    }
    if (a_check_frame(handle->decode[1].diff_us, IR_REMOTE_CHECK_REPEAT) != 0)            /* check repeat */
    {
        if (handle->receive_callback != NULL)                                             /* check the receive callback */
        {
            data.address = 0x00;                                                          /* set address 0x00 */
            data.command = 0x00;                                                          /* set command 0x00 */
            data.status = IR_REMOTE_STATUS_FRAME_INVALID;                                 /* frame invalid */
            handle->receive_callback(&data);                                              /* run the callback */
        }
        
        return;                                                                           /* return */
    }
    if (a_check_frame(handle->decode[2].diff_us, IR_REMOTE_CHECK_STOP) != 0)              /* check stop */
    {
        if (handle->receive_callback != NULL)                                             /* check the receive callback */
        {
            data.address = 0x00;                                                          /* set address 0x00 */
            data.command = 0x00;                                                          /* set command 0x00 */
            data.status = IR_REMOTE_STATUS_FRAME_INVALID;                                 /* frame invalid */
            handle->receive_callback(&data);                                              /* run the callback */
        }
        
        return;                                                                           /* return */
    }
    
    if (handle->receive_callback != NULL)                                                 /* check the receive callback */
    {
        data.address = handle->last_code.address;                                         /* set address 0x00 */
        data.command = handle->last_code.command;                                         /* set command 0x00 */
        data.status = IR_REMOTE_STATUS_REPEAT;                                            /* frame invalid */
        handle->receive_callback(&data);                                                  /* run the callback */
    }
}

/**
 * @brief     ir_remote nec decode
 * @param[in] *handle points to an ir_remote handle structure
 * @note      none
 */
static void a_ir_remote_nec_decode(ir_remote_handle_t *handle)
{
    uint8_t tmp;
    uint8_t tmp_r;
    uint8_t tmp_cmp;
    uint16_t i;
    uint16_t len;
    ir_remote_t data;
    
    len = handle->decode_len - 1;                                                                             /* len - 1 */
    for (i = 0; i < len; i++)                                                                                 /* diff all time */
    {
        int64_t diff;
        
        diff = (int64_t)((int64_t)handle->decode[i + 1].t.s -
               (int64_t)handle->decode[i].t.s) * 1000000 + 
               (int64_t)((int64_t)handle->decode[i + 1].t.us -
               (int64_t)handle->decode[i].t.us);                                                              /* diff time */
        handle->decode[i].diff_us = (uint32_t)diff;                                                           /* save the time diff */
    }
    
    if (a_check_frame(handle->decode[0].diff_us, IR_REMOTE_CHECK_START_HIGH) != 0)                            /* check start diff */
    {
        if (handle->receive_callback != NULL)                                                                 /* check the receive callback */
        {
            data.address = 0x00;                                                                              /* set address 0x00 */
            data.command = 0x00;                                                                              /* set command 0x00 */
            data.status = IR_REMOTE_STATUS_FRAME_INVALID;                                                     /* frame invalid */
            handle->receive_callback(&data);                                                                  /* run the callback */
        }
        
        return;                                                                                               /* return */
    }
    if (a_check_frame(handle->decode[1].diff_us, IR_REMOTE_CHECK_START_LOW) != 0)                             /* check start low */
    {
        if (handle->receive_callback != NULL)                                                                 /* check the receive callback */
        {
            data.address = 0x00;                                                                              /* set address 0x00 */
            data.command = 0x00;                                                                              /* set command 0x00 */
            data.status = IR_REMOTE_STATUS_FRAME_INVALID;                                                     /* frame invalid */
            handle->receive_callback(&data);                                                                  /* run the callback */
        }
        
        return;                                                                                               /* return */
    }
    
    tmp = 0;                                                                                                  /* init 0 */
    for (i = 0; i < 8; i++)                                                                                   /* parse 8 bit */
    {
        if (a_check_frame(handle->decode[2 + i * 2 + 0].diff_us, IR_REMOTE_CHECK_DATA1_HIGH) != 0)            /* check data high */
        {
            if (handle->receive_callback != NULL)                                                             /* check the receive callback */
            {
                data.address = 0x00;                                                                          /* set address 0x00 */
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_ADDR_ERR;                                                      /* address error */
                handle->receive_callback(&data);                                                              /* run the callback */
            }
            
            return;                                                                                           /* return */
        }
        if (handle->decode[2 + i * 2 + 1].diff_us > IR_REMOTE_CHECK_DATA_0_1_EDGE)                            /* check data0 and data1 */
        {
            if (a_check_frame(handle->decode[2 + i * 2 + 1].diff_us, IR_REMOTE_CHECK_DATA1_LOW) != 0)         /* check data 1 */
            {
                data.address = 0x00;                                                                          /* set address 0x00 */
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_ADDR_ERR;                                                      /* address error */
                if (handle->receive_callback != NULL)                                                         /* check the receive callback */
                {
                    handle->receive_callback(&data);                                                          /* run the callback */
                }
                
                return;                                                                                       /* return */
            }
            tmp |= 1 << i;                                                                                    /* set bit */
        }
        else                                                                                                  /* check data 0 */
        {
            if (a_check_frame(handle->decode[2 + i * 2 + 1].diff_us, IR_REMOTE_CHECK_DATA0_LOW) != 0)         /* check data 0 */
            {
                data.address = 0x00;                                                                          /* set address 0x00 */
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_ADDR_ERR;                                                      /* address error */
                if (handle->receive_callback != NULL)                                                         /* check the receive callback */
                {
                    handle->receive_callback(&data);                                                          /* run the callback */
                }
                
                return;                                                                                       /* return */
            }
            tmp |= 0 << i;                                                                                    /* set bit */
        }
    }
    tmp_r = 0;                                                                                                /* init 0 */
    for (i = 0; i < 8; i++)                                                                                   /* parse 8 bit */
    {
        if (a_check_frame(handle->decode[18 + i * 2 + 0].diff_us, IR_REMOTE_CHECK_DATA1_HIGH) != 0)           /* check data high */
        {
            if (handle->receive_callback != NULL)                                                             /* check the receive callback */
            {
                data.address = 0x00;                                                                          /* set address 0x00 */
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_ADDR_ERR;                                                      /* address error */
                handle->receive_callback(&data);                                                              /* run the callback */
            }
            
            return;                                                                                           /* return */
        }
        if (handle->decode[18 + i * 2 + 1].diff_us > IR_REMOTE_CHECK_DATA_0_1_EDGE)                           /* check data0 and data1 */
        {
            if (a_check_frame(handle->decode[18 + i * 2 + 1].diff_us, IR_REMOTE_CHECK_DATA1_LOW) != 0)        /* check data 1 */
            {
                data.address = 0x00;                                                                          /* set address 0x00 */
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_ADDR_ERR;                                                      /* address error */
                if (handle->receive_callback != NULL)                                                         /* check the receive callback */
                {
                    handle->receive_callback(&data);                                                          /* run the callback */
                }
                
                return;                                                                                       /* return */
            }
            tmp_r |= 1 << i;                                                                                  /* set bit */
        }
        else                                                                                                  /* check data 0 */
        {
            if (a_check_frame(handle->decode[18 + i * 2 + 1].diff_us, IR_REMOTE_CHECK_DATA0_LOW) != 0)        /* check data 0 */
            {
                data.address = 0x00;                                                                          /* set address 0x00 */
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_ADDR_ERR;                                                      /* address error */
                if (handle->receive_callback != NULL)                                                         /* check the receive callback */
                {
                    handle->receive_callback(&data);                                                          /* run the callback */
                }
                
                return;                                                                                       /* return */
            }
            tmp_r |= 0 << i;                                                                                  /* set bit */
        }
    }
    tmp_cmp = ~tmp_r;                                                                                         /* get the check value */
    if (tmp != tmp_cmp)                                                                                       /* check the value */
    {
        data.address = 0x00;                                                                                  /* set address 0x00 */
        data.command = 0x00;                                                                                  /* set command 0x00 */
        data.status = IR_REMOTE_STATUS_ADDR_ERR;                                                              /* address error */
        if (handle->receive_callback != NULL)                                                                 /* check the receive callback */
        {
            handle->receive_callback(&data);                                                                  /* run the callback */
        }
        
        return;                                                                                               /* return */
    }
    data.address = tmp;                                                                                       /* set the address */
    
    tmp = 0;                                                                                                  /* init 0 */
    for (i = 0; i < 8; i++)                                                                                   /* parse 8 bit */
    {
        if (a_check_frame(handle->decode[34 + i * 2 + 0].diff_us, IR_REMOTE_CHECK_DATA1_HIGH) != 0)           /* check data high */
        {
            if (handle->receive_callback != NULL)                                                             /* check the receive callback */
            {
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_CMD_ERR;                                                       /* command error */
                handle->receive_callback(&data);                                                              /* run the callback */
            }
            
            return;                                                                                           /* return */
        }
        if (handle->decode[34 + i * 2 + 1].diff_us > IR_REMOTE_CHECK_DATA_0_1_EDGE)                           /* check data0 and data1 */
        {
            if (a_check_frame(handle->decode[34 + i * 2 + 1].diff_us, IR_REMOTE_CHECK_DATA1_LOW) != 0)        /* check data 1 */
            {
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_CMD_ERR;                                                       /* command error */
                if (handle->receive_callback != NULL)                                                         /* check the receive callback */
                {
                    handle->receive_callback(&data);                                                          /* run the callback */
                }
                
                return;                                                                                       /* return */
            }
            tmp |= 1 << i;                                                                                    /* set bit */
        }
        else                                                                                                  /* check data 0 */
        {
            if (a_check_frame(handle->decode[34 + i * 2 + 1].diff_us, IR_REMOTE_CHECK_DATA0_LOW) != 0)        /* check data 0 */
            {
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_CMD_ERR;                                                       /* command error */
                if (handle->receive_callback != NULL)                                                         /* check the receive callback */
                {
                    handle->receive_callback(&data);                                                          /* run the callback */
                }
                
                return;                                                                                       /* return */
            }
            tmp |= 0 << i;                                                                                    /* set bit */
        }
    }
    tmp_r = 0;                                                                                                /* init 0 */
    for (i = 0; i < 8; i++)                                                                                   /* parse 8 bit */
    {
        if (a_check_frame(handle->decode[50 + i * 2 + 0].diff_us, IR_REMOTE_CHECK_DATA1_HIGH) != 0)           /* check data high */
        {
            if (handle->receive_callback != NULL)                                                             /* check the receive callback */
            {
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_CMD_ERR;                                                       /* command error */
                handle->receive_callback(&data);                                                              /* run the callback */
            }
            
            return;                                                                                           /* return */
        }
        if (handle->decode[50 + i * 2 + 1].diff_us > IR_REMOTE_CHECK_DATA_0_1_EDGE)                           /* check data0 and data1 */
        {
            if (a_check_frame(handle->decode[50 + i * 2 + 1].diff_us, IR_REMOTE_CHECK_DATA1_LOW) != 0)        /* check data 1 */
            {
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_CMD_ERR;                                                       /* command error */
                if (handle->receive_callback != NULL)                                                         /* check the receive callback */
                {
                    handle->receive_callback(&data);                                                          /* run the callback */
                }
                
                return;                                                                                       /* return */
            }
            tmp_r |= 1 << i;                                                                                  /* set bit */
        }
        else                                                                                                  /* check data 0 */
        {
            if (a_check_frame(handle->decode[50 + i * 2 + 1].diff_us, IR_REMOTE_CHECK_DATA0_LOW) != 0)        /* check data 0 */
            {
                data.command = 0x00;                                                                          /* set command 0x00 */
                data.status = IR_REMOTE_STATUS_CMD_ERR;                                                       /* command error */
                if (handle->receive_callback != NULL)                                                         /* check the receive callback */
                {
                    handle->receive_callback(&data);                                                          /* run the callback */
                }
                
                return;                                                                                       /* return */
            }
            tmp_r |= 0 << i;                                                                                  /* set bit */
        }
    }
    tmp_cmp = ~tmp_r;                                                                                         /* get the check value */
    if (tmp != tmp_cmp)                                                                                       /* check the value */
    {
        data.command = 0x00;                                                                                  /* set command 0x00 */
        data.status = IR_REMOTE_STATUS_CMD_ERR;                                                               /* command error */
        if (handle->receive_callback != NULL)                                                                 /* check the receive callback */
        {
            handle->receive_callback(&data);                                                                  /* run the callback */
        }
        
        return;                                                                                               /* return */
    }
    data.command = tmp;                                                                                       /* set the command */
    
    if (a_check_frame(handle->decode[66].diff_us, IR_REMOTE_CHECK_STOP) != 0)                                 /* check stop frame */
    {
        if (handle->receive_callback != NULL)                                                                 /* check the receive callback */
        {
            data.status = IR_REMOTE_STATUS_FRAME_INVALID;                                                     /* frame invalid */
            handle->receive_callback(&data);                                                                  /* run the callback */
        }
        
        return;                                                                                               /* return */
    }
    
    if (handle->receive_callback != NULL)                                                                     /* check the receive callback */
    {
        data.status = IR_REMOTE_STATUS_OK;                                                                    /* frame ok */
        handle->receive_callback(&data);                                                                      /* run the callback */
    }
    handle->last_code.address = data.address;                                                                 /* save address */
    handle->last_code.command = data.command;                                                                 /* save command */
    handle->last_code.status = data.status;                                                                   /* save status */
    handle->decode_len = 0;                                                                                   /* clear the buffer */
}

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
uint8_t ir_remote_irq_handler(ir_remote_handle_t *handle)
{
    uint8_t res;
    int64_t diff;
    ir_remote_time_t t;
    
    if (handle == NULL)                                                    /* check handle */
    {
        return 2;                                                          /* return error */
    }
    if (handle->inited != 1)                                               /* check handle initialization */
    {
        return 3;                                                          /* return error */
    }
    
    res = handle->timestamp_read(&t);                                      /* timestamp read */
    if (res != 0)                                                          /* check result */
    {
        handle->debug_print("ir_remote: timestamp read failed.\n");        /* timestamp read failed */
        
        return 1;                                                          /* return error */
    }
    diff = (int64_t)(t.s - handle->last_time.s) * 1000000 + 
           (int64_t)(t.us - handle->last_time.us) ;                        /* now - last time */
    if (diff - (int64_t)200000L >= 0)                                      /* if over 1s, force reset */
    {
        handle->decode_len = 0;                                            /* reset the decode */
    }
    if (handle->decode_len >= 127)                                         /* check the max length */
    {
        handle->decode_len = 0;                                            /* reset the decode */
    }
    handle->decode[handle->decode_len].t.s = t.s;                          /* save s */
    handle->decode[handle->decode_len].t.us = t.us;                        /* save us */
    handle->decode_len++;                                                  /* length++ */
    handle->last_time.s = t.s;                                             /* save last time */
    handle->last_time.us = t.us;                                           /* save last time */
    if (handle->decode_len >= 68)                                          /* check the end length */
    {
        diff = (int64_t)((int64_t)handle->decode[2].t.s -
               (int64_t)handle->decode[1].t.s) * 1000000 + 
               (int64_t)((int64_t)handle->decode[2].t.us -
               (int64_t)handle->decode[1].t.us);                           /* diff time */
        if (a_check_frame((uint16_t)diff, IR_REMOTE_CHECK_START_LOW) == 0) /* check the frame */
        {
            a_ir_remote_nec_decode(handle);                                /* try to decode */
        }
    }
    if (handle->decode_len == 4)                                           /* check the end length */
    {
        diff = (int64_t)((int64_t)handle->decode[2].t.s -
               (int64_t)handle->decode[1].t.s) * 1000000 + 
               (int64_t)((int64_t)handle->decode[2].t.us -
               (int64_t)handle->decode[1].t.us);                           /* diff time */
        if (a_check_frame((uint16_t)diff, IR_REMOTE_CHECK_REPEAT) == 0)    /* check the frame */
        {
            a_ir_remote_nec_repeat_decode(handle);                         /* try to decode */
        }
    }
    
    return 0;                                                              /* success return 0 */
}

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
uint8_t ir_remote_init(ir_remote_handle_t *handle)
{
    uint8_t res;
    ir_remote_time_t t;
    
    if (handle == NULL)                                                       /* check handle */
    {
        return 2;                                                             /* return error */
    }
    if (handle->debug_print == NULL)                                          /* check debug_print */
    {
        return 3;                                                             /* return error */
    }
    if (handle->timestamp_read == NULL)                                       /* check timestamp_read */
    {
        handle->debug_print("ir_remote: timestamp_read is null.\n");          /* timestamp_read is null */
        
        return 3;                                                             /* return error */
    }
    if (handle->delay_ms == NULL)                                             /* check delay_ms */
    {
        handle->debug_print("ir_remote: delay_ms is null.\n");                /* delay_ms is null */
        
        return 3;                                                             /* return error */
    }
    if (handle->receive_callback == NULL)                                     /* check receive_callback */
    {
        handle->debug_print("ir_remote: receive_callback is null.\n");        /* receive_callback is null */
        
        return 3;                                                             /* return error */
    }
    
    res = handle->timestamp_read(&t);                                         /* timestamp read */
    if (res != 0)                                                             /* check result */
    {
        handle->debug_print("ir_remote: timestamp read failed.\n");           /* timestamp read failed */
        
        return 1;                                                             /* return error */
    }
    handle->last_time.s = t.s;                                                /* save last time */
    handle->last_time.us = t.us;                                              /* save last time */
    handle->last_code.address = 0x00;                                         /* init address 0 */
    handle->last_code.command = 0x00;                                         /* init command 0 */
    handle->last_code.status = 0x00;                                          /* init status 0 */
    handle->decode_len = 0;                                                   /* init 0 */
    handle->inited = 1;                                                       /* flag inited */
    
    return 0;                                                                 /* success return 0 */
}

/**
 * @brief     close the chip
 * @param[in] *handle points to an ir_remote handle structure
 * @return    status code
 *            - 0 success
 *            - 2 handle is NULL
 *            - 3 handle is not initialized
 * @note      none
 */
uint8_t ir_remote_deinit(ir_remote_handle_t *handle)
{
    if (handle == NULL)             /* check handle */
    {
        return 2;                   /* return error */
    }
    if (handle->inited != 1)        /* check handle initialization */
    {
        return 3;                   /* return error */
    }
    
    handle->inited = 0;             /* flag close */
    
    return 0;                       /* success return 0 */
}

/**
 * @brief      get chip's information
 * @param[out] *info points to an ir_remote info structure
 * @return     status code
 *             - 0 success
 *             - 2 handle is NULL
 * @note       none
 */
uint8_t ir_remote_info(ir_remote_info_t *info)
{
    if (info == NULL)                                               /* check handle */
    {
        return 2;                                                   /* return error */
    }
    
    memset(info, 0, sizeof(ir_remote_info_t));                      /* initialize ir_remote info structure */
    strncpy(info->chip_name, CHIP_NAME, 32);                        /* copy chip name */
    strncpy(info->manufacturer_name, MANUFACTURER_NAME, 32);        /* copy manufacturer name */
    strncpy(info->interface, "GPIO", 8);                            /* copy interface name */
    info->supply_voltage_min_v = SUPPLY_VOLTAGE_MIN;                /* set minimal supply voltage */
    info->supply_voltage_max_v = SUPPLY_VOLTAGE_MAX;                /* set maximum supply voltage */
    info->max_current_ma = MAX_CURRENT;                             /* set maximum current */
    info->temperature_max = TEMPERATURE_MAX;                        /* set minimal temperature */
    info->temperature_min = TEMPERATURE_MIN;                        /* set maximum temperature */
    info->driver_version = DRIVER_VERSION;                          /* set driver version */
    
    return 0;                                                       /* success return 0 */
}
