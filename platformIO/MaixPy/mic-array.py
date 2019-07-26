from Maix import MIC_ARRAY as mic
import lcd

lcd.init()
mic.init()
# reconfigure pins after mic.init() to match your wiring
fpioa.set_function(32, fpioa.I2S0_IN_D0);
fpioa.set_function(15, fpioa.I2S0_IN_D1);
fpioa.set_function(23, fpioa.I2S0_IN_D2);
fpioa.set_function(24, fpioa.I2S0_IN_D3);
fpioa.set_function(21, fpioa.I2S0_WS);
fpioa.set_function(22, fpioa.I2S0_SCLK);
fpioa.set_function(13, fpioa.GPIOHS0 + 27);
fpioa.set_function(14, fpioa.GPIOHS0 + 28);

while True:
    imga = mic.get_map()
    b = mic.get_dir(imga)
    a = mic.set_led(b,(0,0,255))
    imgb = imga.resize(240,160)
    imgc = imgb.to_rainbow(1)
    a = lcd.display(imgc)
mic.deinit()
