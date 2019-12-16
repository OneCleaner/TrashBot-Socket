from adafruit_servokit import ServoKit
import RPi.GPIO as gpio
from time import sleep

a = 12
d = 6

gpio.setwarnings(False)
gpio.setmode(gpio.BCM)
gpio.setup(d, gpio.OUT)
gpio.setup(a, gpio.OUT)
gpio.output(d, gpio.LOW)
gpio.output(a, gpio.LOW)

const = 1.6
fract = const / 32
kit = ServoKit(channels=16)

while 1:
    gpio.output(d, gpio.HIGH)
    gpio.output(a, gpio.LOW)
    sleep(const)
    gpio.output(a, gpio.HIGH)

    for i in range(0, 160):
        kit.servo[4].angle = i
        sleep(fract)

    gpio.output(d, gpio.LOW)
    gpio.output(a, gpio.HIGH)
    sleep(const)
    gpio.output(d, gpio.HIGH)

    for i in range(160, 0, -1):
        kit.servo[4].angle = i
        sleep(fract)
