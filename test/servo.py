from adafruit_servokit import ServoKit

kit = ServoKit(channels=16)

kit.servo[4].angle = 100   # base
kit.servo[3].angle = 80    # n.0
kit.servo[12].angle = 0    # rotazione pinza
kit.servo[8].angle = 0     # non funziona
kit.servo[2].angle = 60    # rotazione base