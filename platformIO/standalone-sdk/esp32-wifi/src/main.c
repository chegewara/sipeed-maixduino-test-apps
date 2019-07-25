/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// #include <stdio.h>
#include <syslog.h>
#include <fpioa.h>
#include <sleep.h>
#include <bsp.h>

#include "esp32_spi.h"
#include "esp32_spi_io.h"

#define ESP32_CS        FUNC_GPIOHS10
#define ESP32_RST       FUNC_GPIOHS11
#define ESP32_RDY       FUNC_GPIOHS12
#define ESP32_MOSI      FUNC_GPIOHS13
#define ESP32_MISO      FUNC_GPIOHS14
#define ESP32_SCLK      FUNC_GPIOHS15

#define SSID "sipeed"
#define PASS "12345678"

static uint8_t read_buf[0xffff] = {0};

static void init_spi()
{
    /* INIT SPI */
    // #iomap at MaixDuino
    fpioa_set_function(25, ESP32_CS);
    fpioa_set_function(8, ESP32_RST);
    fpioa_set_function(9, ESP32_RDY);
    fpioa_set_function(28, ESP32_MOSI);
    fpioa_set_function(26, ESP32_MISO);
    fpioa_set_function(27, ESP32_SCLK);

    esp32_spi_config_io(ESP32_CS - FUNC_GPIOHS0, ESP32_RST, ESP32_RDY - FUNC_GPIOHS0,
                        ESP32_MOSI - FUNC_GPIOHS0, ESP32_MISO - FUNC_GPIOHS0, ESP32_SCLK - FUNC_GPIOHS0);

    esp32_spi_init();
}

static void scan_WiFi()
{
    esp32_spi_aps_list_t *scan = esp32_spi_scan_networks();
    esp32_spi_ap_t *aps;
    for(int i=0;i<scan->aps_num;i++)
    {
        aps = scan->aps[i];
        printf("SSID: %s, RSSI: %d, ENC: %d\r\n", aps->ssid, aps->rssi, aps->encr);
    }
}

static int connect_AP(uint8_t* ssid, uint8_t* pass)
{
    int status = esp32_spi_connect_AP(ssid, pass, 10);
    printf("Connecting to AP status: %d\r\n", status);

    return status;
}

static void test_connection()
{
    uint16_t time = esp32_spi_ping((uint8_t*)"sipeed.com", 1, 100);
    printf("time: %dms\r\n", time);
    
    uint8_t ip[4];
    esp32_spi_get_host_by_name((uint8_t*)"sipeed.com", ip);
    uint8_t str_ip[20];
    esp32_spi_pretty_ip(ip, str_ip);
    printf("IP: %s\r\n", str_ip);
}

static void test_socket()
{
    _DEBUG();
    uint8_t socket = connect_server_port("dl.sipeed.com", 80);
    if(socket == 0xff)
        return;

    int status = esp32_spi_socket_status(socket);
    printf("open socket status: %s\r\n", socket_enum_to_str(status));
    #define LEN 1024 // its max buffer length with which esp32 can read fast enough from internet

    if(status == SOCKET_ESTABLISHED)
    {
        char* buf = "GET /MAIX/MaixPy/assets/Alice.jpg HTTP/1.1\r\nHost: dl.sipeed.com\r\ncache-control: no-cache\r\n\r\n";
        uint32_t len = esp32_spi_socket_write(socket, (uint8_t*)buf, strlen(buf)+1);
        printf("len value: %d\r\n", len);
        int total = 0;

        msleep(300); // small delay required between write and read

        if(len > 0)
        {
            uint16_t len1 = 0;            
            uint8_t tmp_buf[LEN] = {0};
            do{
                len = esp32_spi_socket_available(socket);
                printf("bytes available %d\r\n", len);
                len1 = esp32_spi_socket_read(socket, &tmp_buf[0], len > LEN ? LEN:len);
                strncat((char*)read_buf, (char*)tmp_buf, len1);
                total += len1;
                status = esp32_spi_socket_status(socket);
                msleep(65);
           }while(len > LEN && status != -1);

            printf("total data read len: %d\r\n", total);
        }
    }

    status = esp32_spi_socket_close(socket);
    printf("close socket status: %s\r\n", socket_enum_to_str(status));
}

int main(void)
{
    LOGI(__func__, "Kendryte "__DATE__ " " __TIME__ "\r\n");
    LOGI(__func__, "ESP32 test app\r\n");

    init_spi();
    /* Get esp32 firmware version */
    printf("version: %s\r\n", esp32_spi_firmware_version());

    scan_WiFi();
    while(connect_AP(SSID, PASS) != 0);

    test_connection();

    test_socket();
    while(1)
    {
        msleep(1000);
    }
}
