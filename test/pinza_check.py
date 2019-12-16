import RPi.GPIO as gpio
from time import time
d = 6
a = 12

gpio.setwarnings(False)
gpio.setmode(gpio.BCM)
gpio.setup(d, gpio.OUT)
gpio.setup(a, gpio.OUT)
gpio.output(d, gpio.LOW)
gpio.output(a, gpio.LOW)

while 1:
    input()
    start = time()
    gpio.output(d, gpio.HIGH)
    gpio.output(a, gpio.LOW)
    input()
    gpio.output(a, gpio.HIGH)
    end = time()
    print(end - start)
    input()
    start = time()
    gpio.output(a, gpio.HIGH)
    gpio.output(d, gpio.LOW)
    input()
    gpio.output(d, gpio.HIGH)
    end = time()
    print(end - start)
    gpio.output(d, gpio.LOW)
    gpio.output(a, gpio.LOW)