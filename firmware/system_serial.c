/*
 * RaceCapture/Pro stimulator firmware
 *
 * Copyright (C) 2017 Autosport Labs
 *
 * This file is part of the Race Capture firmware suite
 *
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should
 * have received a copy of the GNU General Public License along with
 * this code. If not, see <http://www.gnu.org/licenses/>.
 */
#include "system_serial.h"
#include "logging.h"
#include "time_util.h"
uint8_t test_buffer[1024] = "";

/*
 * Read a line from the specified serial connection into the specified
 * buffer buf with a length of buf_len
 * This call blocks until a \n or buf_len is reached.
 */
size_t serial_getline(SerialDriver *sdp, uint8_t *buf, size_t buf_len)
{
        if (!buf || !buf_len)
                return 0;

        size_t read = 0;
        --buf_len; /* account for NULL terminator */
        while (read < buf_len) {
                ++read;
                *buf++ = sdGet(sdp);
                if ('\r' == buf[-1]) {
                        buf--;
                        break;
                }
        }
        *buf = '\0';
        return read;
}


void system_serial_init_SD1(uint32_t speed)
{
    static SerialConfig uart_cfg;
    uart_cfg.speed=speed;

    /* USART1 TX.       */
    palSetPadMode(GPIOA, 9, PAL_STM32_MODE_ALTERNATE | PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_ALTERNATE(1));
    /* USART1 RX.       */
    palSetPadMode(GPIOA, 10, PAL_STM32_MODE_ALTERNATE | PAL_STM32_PUPDR_PULLUP | PAL_STM32_ALTERNATE(1));
    sdStart(&SD1, &uart_cfg);
}

/*
 * Initialize connection for SD2 (STN1110)
 */
void system_serial_init_SD2(uint32_t speed)
{
        static SerialConfig uart_cfg;
        uart_cfg.speed=speed;

        /* USART2 TX.       */
        palSetPadMode(GPIOA, 2, PAL_STM32_MODE_ALTERNATE | PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_ALTERNATE(1));
        /* USART2 RX.       */
        palSetPadMode(GPIOA, 3, PAL_STM32_MODE_ALTERNATE | PAL_STM32_PUPDR_PULLUP | PAL_STM32_ALTERNATE(1));
        sdStart(&SD2, &uart_cfg);
}

/* Initialize our serial subsystem */
void system_serial_init()
{
        system_serial_init_SD2(SD2_BAUD);
        system_serial_init_SD1(SD1_BAUD);
}

uint8_t * get_test_buffer(void)
{
        return test_buffer;
}

void system_serial_worker(void)
{
        log_info("starting serial worker\r\n");
        while(!chThdShouldTerminateX()) {
                serial_getline(&SD1, test_buffer, sizeof(test_buffer));
                log_info("Serial Worker echo %s\r\n", test_buffer);
        }
}


