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
 * @file      main.c
 * @brief     main source file
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
#include "driver_ir_remote_receive_test.h"
#include "shell.h"
#include "clock.h"
#include "delay.h"
#include "gpio.h"
#include "uart.h"
#include "getopt.h"
#include <stdlib.h>

/**
 * @brief global var definition
 */
uint8_t g_buf[256];                                         /**< uart buffer */
volatile uint16_t g_len;                                    /**< uart buffer length */
volatile uint8_t g_flag;                                    /**< interrupt flag */
uint8_t (*g_gpio_irq)(void) = NULL;                         /**< gpio irq */
extern uint8_t ir_remote_interface_timer_init(void);        /**< timer init function */

/**
 * @brief exti 0 irq
 * @note  none
 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief     gpio exti callback
 * @param[in] pin is the gpio pin
 * @note      none
 */
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == GPIO_PIN_0)
    {
        /* run the gpio riq */
        if (g_gpio_irq != NULL)
        {
            g_gpio_irq();
        }
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
            g_flag = 1;
            
            break;
        }
        case IR_REMOTE_STATUS_REPEAT :
        {
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
 * @brief     ir_remote full function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *            - 0 success
 *            - 1 run failed
 *            - 5 param is invalid
 * @note      none
 */
uint8_t ir_remote(uint8_t argc, char **argv)
{
    int c;
    int longindex = 0;
    char short_options[] = "hipe:t:";
    struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"information", no_argument, NULL, 'i'},
        {"port", no_argument, NULL, 'p'},
        {"example", required_argument, NULL, 'e'},
        {"test", required_argument, NULL, 't'},
        {"times", required_argument, NULL, 1},
        {NULL, 0, NULL, 0},
    };
    char type[33] = "unknown";
    uint32_t times = 3;
    
    /* if no params */
    if (argc == 1)
    {
        /* goto the help */
        goto help;
    }

    /* init 0 */
    optind = 0;

    /* parse */
    do
    {
        /* parse the args */
        c = getopt_long(argc, argv, short_options, long_options, &longindex);

        /* judge the result */
        switch (c)
        {
            /* help */
            case 'h' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "h");

                break;
            }

            /* information */
            case 'i' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "i");

                break;
            }

            /* port */
            case 'p' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "p");

                break;
            }

            /* example */
            case 'e' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "e_%s", optarg);

                break;
            }

            /* test */
            case 't' :
            {
                /* set the type */
                memset(type, 0, sizeof(char) * 33);
                snprintf(type, 32, "t_%s", optarg);

                break;
            }

            /* running times */
            case 1 :
            {
                /* set the times */
                times = atol(optarg);

                break;
            }

            /* the end */
            case -1 :
            {
                break;
            }

            /* others */
            default :
            {
                return 5;
            }
        }
    } while (c != -1);

    /* run the function */
    if (strcmp("t_read", type) == 0)
    {
        uint8_t res;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* set the irq */
        g_gpio_irq = ir_remote_receive_test_irq_handler;
        
        /* run the receive test */
        res = ir_remote_receive_test(times);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
        }
        
        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("e_read", type) == 0)
    {
        uint8_t res;
        uint32_t i;
        uint16_t timeout;
        
        /* gpio init */
        res = gpio_interrupt_init();
        if (res != 0)
        {
            return 1;
        }
        
        /* set the irq */
        g_gpio_irq = ir_remote_basic_irq_handler;
        
        /* basic init */
        res = ir_remote_basic_init(a_receive_callback);
        if (res != 0)
        {
            (void)gpio_interrupt_deinit();
            g_gpio_irq = NULL;
        }
        
        /* loop */
        for (i = 0; i < times; i++)
        {
            /* output */
            ir_remote_interface_debug_print("%d/%d.\n", i + 1, times);
            
            /* 5s timeout */
            timeout = 500;
            
            /* init 0 */
            g_flag = 0;
            
            /* check timeout */
            while (timeout != 0)
            {
                /* check the flag */
                if (g_flag != 0)
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
                (void)ir_remote_basic_deinit();
                (void)gpio_interrupt_deinit();
                g_gpio_irq = NULL;
                
                return 1;
            }
        }
        
        /* basic deinit */
        (void)ir_remote_basic_deinit();
        
        /* gpio deinit */
        (void)gpio_interrupt_deinit();
        g_gpio_irq = NULL;
        
        return 0;
    }
    else if (strcmp("h", type) == 0)
    {
        help:
        ir_remote_interface_debug_print("Usage:\n");
        ir_remote_interface_debug_print("  ir_remote (-i | --information)\n");
        ir_remote_interface_debug_print("  ir_remote (-h | --help)\n");
        ir_remote_interface_debug_print("  ir_remote (-p | --port)\n");
        ir_remote_interface_debug_print("  ir_remote (-t read | --test=read) [--times=<num>]\n");
        ir_remote_interface_debug_print("  ir_remote (-e read | --example=read) [--times=<num>]\n");
        ir_remote_interface_debug_print("\n");
        ir_remote_interface_debug_print("Options:\n");
        ir_remote_interface_debug_print("  -e <read>, --example=<read>    Run the driver example.\n");
        ir_remote_interface_debug_print("  -h, --help                     Show the help.\n");
        ir_remote_interface_debug_print("  -i, --information              Show the chip information.\n");
        ir_remote_interface_debug_print("  -p, --port                     Display the pin connections of the current board.\n");
        ir_remote_interface_debug_print("  -t <read>, --test=<read>       Run the driver test.\n");
        ir_remote_interface_debug_print("      --times=<num>              Set the running times.([default: 3])\n");

        return 0;
    }
    else if (strcmp("i", type) == 0)
    {
        ir_remote_info_t info;

        /* print ir_remote info */
        ir_remote_info(&info);
        ir_remote_interface_debug_print("ir_remote: chip is %s.\n", info.chip_name);
        ir_remote_interface_debug_print("ir_remote: manufacturer is %s.\n", info.manufacturer_name);
        ir_remote_interface_debug_print("ir_remote: interface is %s.\n", info.interface);
        ir_remote_interface_debug_print("ir_remote: driver version is %d.%d.\n", info.driver_version / 1000, (info.driver_version % 1000) / 100);
        ir_remote_interface_debug_print("ir_remote: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
        ir_remote_interface_debug_print("ir_remote: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
        ir_remote_interface_debug_print("ir_remote: max current is %0.2fmA.\n", info.max_current_ma);
        ir_remote_interface_debug_print("ir_remote: max temperature is %0.1fC.\n", info.temperature_max);
        ir_remote_interface_debug_print("ir_remote: min temperature is %0.1fC.\n", info.temperature_min);

        return 0;
    }
    else if (strcmp("p", type) == 0)
    {
        /* print pin connection */
        ir_remote_interface_debug_print("ir_remote: INT connected to GPIOB PIN0.\n");

        return 0;
    }
    else
    {
        return 5;
    }
}

/**
 * @brief main function
 * @note  none
 */
int main(void)
{
    uint8_t res;

    /* stm32f407 clock init and hal init */
    clock_init();

    /* delay init */
    delay_init();

    /* uart init */
    uart_init(115200);
    
    /* timer init */
    ir_remote_interface_timer_init();
    
    /* shell init && register ir_remote function */
    shell_init();
    shell_register("ir_remote", ir_remote);
    uart_print("ir_remote: welcome to libdriver ir_remote.\n");
    
    while (1)
    {
        /* read uart */
        g_len = uart_read(g_buf, 256);
        if (g_len != 0)
        {
            /* run shell */
            res = shell_parse((char *)g_buf, g_len);
            if (res == 0)
            {
                /* run success */
            }
            else if (res == 1)
            {
                uart_print("ir_remote: run failed.\n");
            }
            else if (res == 2)
            {
                uart_print("ir_remote: unknown command.\n");
            }
            else if (res == 3)
            {
                uart_print("ir_remote: length is too long.\n");
            }
            else if (res == 4)
            {
                uart_print("ir_remote: pretreat failed.\n");
            }
            else if (res == 5)
            {
                uart_print("ir_remote: param is invalid.\n");
            }
            else
            {
                uart_print("ir_remote: unknown status code.\n");
            }
            uart_flush();
        }
        delay_ms(100);
    }
}
