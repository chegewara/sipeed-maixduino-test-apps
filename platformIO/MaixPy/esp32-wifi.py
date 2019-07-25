/**
 * Simple demo to test network
 */
import socket
import network
import gc
import os
import lcd, image
import machine
from board import board_info
import utime
from Maix import GPIO
from fpioa_manager import *

#iomap at MaixDuino
fm.register(25,fm.fpioa.GPIOHS10)#cs
fm.register(8,fm.fpioa.GPIOHS11)#rst
fm.register(9,fm.fpioa.GPIOHS12)#rdy
fm.register(28,fm.fpioa.GPIOHS13)#mosi
fm.register(26,fm.fpioa.GPIOHS14)#miso
fm.register(27,fm.fpioa.GPIOHS15)#sclk

nic = network.ESP32_SPI(cs=fm.fpioa.GPIOHS10,rst=fm.fpioa.GPIOHS11,rdy=fm.fpioa.GPIOHS12,
mosi=fm.fpioa.GPIOHS13,miso=fm.fpioa.GPIOHS14,sclk=fm.fpioa.GPIOHS15)

wifi = nic.scan()
for v in wifi:
    print("%s\n" %(v), end=" ")
print()

nic.isconnected()
nic.connect("sipeed", "12345678")
nic.isconnected()

sock = socket.socket()
addr = socket.getaddrinfo("dl.sipeed.com", 80)[0][-1]
sock.connect(addr)
sock.send('''GET /MAIX/MaixPy/assets/Alice.jpg HTTP/1.1\r\nHost: dl.sipeed.com\r\ncache-control: no-cache\r\n\r\n''')

utime.sleep_ms(300) # some delay is required here (most likely shorter), to let esp32 gatter some data, or esp32 will return data len 0
img = b""
sock.settimeout(10)
while True:
    data = sock.recv(64)
    if len(data) == 0:
        break
    print("rcv:", len(data))
    img = img + data

print(len(img))
sock.close()
