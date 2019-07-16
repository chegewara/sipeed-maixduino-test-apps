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

int main(void)
{
    LOGI(__func__, "Kendryte "__DATE__ " " __TIME__);
    LOGI(__func__, "ESP32 test app");

    init_spi();
    /* Get esp32 firmware version */
    printf("version: %s\r\n", esp32_spi_firmware_version());

    scan_WiFi();
    connect_AP(SSID, PASS);
    test_connection();

    while(1)
    {
        usleep(1000 * 1000);
    }
}
