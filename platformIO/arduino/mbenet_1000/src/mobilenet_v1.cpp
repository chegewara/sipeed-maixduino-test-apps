
/**
 * 
 * 
 * Download model here: 
 *     http://dl.sipeed.com/MAIX/MaixPy/model/mobilenet_0x300000.kfpkg
 * Unpack it(zip format), get m.kmodel, change name to a short name "m" for example,
 * put it in SD card at root path
 * 
 * 
 * 
 */


#include <Sipeed_OV2640.h>
#include <Sipeed_ST7789.h>
#include "MBNet_1000.h"
#include "Maix_KPU.h"

SPIClass spi_(SPI0); // MUST be SPI0 for Maix series on board LCD
SPIClass spi1(SPI1, 27, 26, 28, 29);
Sd2Card card(spi1);

Sipeed_ST7789 lcd(320, 240, spi_);
Sipeed_OV2640 camera(224, 224, PIXFORMAT_RGB565);
KPUClass KPU;
MBNet_1000 mbnet(KPU, lcd, camera);

const char* kmodel_name = "model";


void setup()
{
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
  if (!card.init(SPI_HALF_SPEED, 29)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }
    
    Serial.println("init mobile net, load kmodel from SD card, it may takes a long time");
    if( mbnet.begin(kmodel_name) != 0)
    {
        Serial.println("mobile net init fail");
        while(1);
    }

}

void loop()
{
    if(mbnet.detect() != 0)
    {
        Serial.println("detect object fail");
        return;
    }
    mbnet.show();
}
