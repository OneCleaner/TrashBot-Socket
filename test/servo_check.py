from adafruit_servokit import ServoKit

kit = ServoKit(channels=16)

while True:
	try:
		x = 0
		y = 0
		while 1:
			x = int(input("numero: "))
			y = int(input("angolo: "))
			kit.servo[x].angle = y
	except Exception as err:
		print(err)
		continue
