[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md) | [日本語](/README_ja.md) | [Deutsch](/README_de.md) | [한국어](/README_ko.md)

<div align=center>
<img src="/doc/image/logo.svg" width="400" height="150"/>
</div>

## LibDriver IR_REMOTE

[![MISRA](https://img.shields.io/badge/misra-compliant-brightgreen.svg)](/misra/README.md) [![API](https://img.shields.io/badge/api-reference-blue.svg)](https://www.libdriver.com/docs/ir_remote/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

The NEC IR transmission protocol uses pulse distance encoding of the message bits. Each pulse burst is 562.5μs in length, at a carrier frequency of 38kHz (26.3μs). Logical bits are transmitted as follows:

Logical '0' – a 562.5μs pulse burst followed by a 562.5μs space, with a total transmit time of 1.125ms;

Logical '1' – a 562.5μs pulse burst followed by a 1.6875ms space, with a total transmit time of 2.25ms.

When a key is pressed on the remote controller, the message transmitted consists of the following in order: 

1. A 9ms leading pulse burst (16 times the pulse burst length used for a logical data bit). 
2. A 4.5ms space.
3. The 8-bit address for the receiving device.
4. The 8-bit logical inverse of the address.
5. The 8-bit command.
6. The 8-bit logical inverse of the command.
7. A final 562.5μs pulse burst to signify the end of message transmission.

LibDriver IR_REMOTE is the full function driver of ir_remote launched by LibDriver. It provides NEC IR remote decoding function. LibDriver is MISRA compliant.

### Table of Contents

  - [Instruction](#Instruction)
  - [Install](#Install)
  - [Usage](#Usage)
    - [example basic](#example-basic)
  - [Document](#Document)
  - [Contributing](#Contributing)
  - [License](#License)
  - [Contact Us](#Contact-Us)

### Instruction

/src includes LibDriver IR_REMOTE source files.

/interface includes LibDriver IR_REMOTE GPIO platform independent template.

/test includes LibDriver IR_REMOTE driver test code and this code can test the chip necessary function simply.

/example includes LibDriver IR_REMOTE sample code.

/doc includes LibDriver IR_REMOTE offline document.

/datasheet includes IR_REMOTE datasheet.

/project includes the common Linux and MCU development board sample code. All projects use the shell script to debug the driver and the detail instruction can be found in each project's README.md.

/misra includes the LibDriver MISRA code scanning results.

### Install

Reference /interface GPIO platform independent template and finish your platform GPIO driver.

Add the /src directory, the interface driver for your platform, and your own drivers to your project, if you want to use the default example drivers, add the /example directory to your project.

### Usage

You can refer to the examples in the /example directory to complete your own driver. If you want to use the default programming examples, here's how to use them.

#### example basic

```C
#include "driver_ir_remote_basic.h"

volatile uint8_t g_flag;                       
uint8_t (*g_gpio_irq)(void) = NULL;                      
uint8_t res;
uint32_t i;
uint16_t timeout;

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

...
    
/* gpio init */
res = gpio_interrupt_init();
if (res != 0)
{
    return 1;
}

/* set the irq */
g_gpio_irq = ir_remote_basic_irq_handler;

...
    
/* basic init */
res = ir_remote_basic_init(a_receive_callback);
if (res != 0)
{
    (void)gpio_interrupt_deinit();
    g_gpio_irq = NULL;
}

...
    
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
    
    ....
}

...
    
/* basic deinit */
(void)ir_remote_basic_deinit();

/* gpio deinit */
(void)gpio_interrupt_deinit();
g_gpio_irq = NULL;

return 0;
```

### Document

Online documents: [https://www.libdriver.com/docs/ir_remote/index.html](https://www.libdriver.com/docs/ir_remote/index.html).

Offline documents: /doc/html/index.html.

### Contributing

Please refer to CONTRIBUTING.md.

### License

Copyright (c) 2015 - present LibDriver All rights reserved



The MIT License (MIT) 



Permission is hereby granted, free of charge, to any person obtaining a copy

of this software and associated documentation files (the "Software"), to deal

in the Software without restriction, including without limitation the rights

to use, copy, modify, merge, publish, distribute, sublicense, and/or sell

copies of the Software, and to permit persons to whom the Software is

furnished to do so, subject to the following conditions: 



The above copyright notice and this permission notice shall be included in all

copies or substantial portions of the Software. 



THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR

IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,

FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE

AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER

LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,

OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE

SOFTWARE. 

### Contact Us

Please send an e-mail to lishifenging@outlook.com.