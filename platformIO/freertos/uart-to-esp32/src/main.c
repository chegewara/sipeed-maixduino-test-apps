#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>

#include "devices.h"
#include "uart.h"
#include "fpioa.h"


int main()
{
    fpioa_set_function(6, FUNC_UART3_TX);
    fpioa_set_function(7, FUNC_UART3_RX);
    handle_t uart = io_open("/dev/uart3");
    uart_config(uart, 115200, 8, UART_STOP_1, UART_PARITY_NONE);
    io_write(uart, "test from k210\r\n", 17);
    char buf;
    while(1){
        while(io_read(uart, &buf, 1))
        {
            printf("%c", buf);
            io_write(uart, &buf, 1);
        }
        vTaskDelay(2);
    }

    vTaskDelete(NULL);
}