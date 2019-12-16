from serial import Serial
import sys






if __name__ == "__main__":

    porta = input("inserisci la porta \n--> ")
    baudrate = input("inserisci il baud \n--> ")
    
    ser = Serial(porta, baudrate)
    
    while True:
        s = input("invia comando \n-->")
        ser.write(s.encode())


    sys.exit()