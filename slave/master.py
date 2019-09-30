from serial import Serial
import sys
import platform


if __name__ == "__main__":
    if platform.system() == "Windows":
        port = "COM" + input("Inserire il numero della porta di arduino \n-> ") #windows
    else:
        port = "/dev/tty" + input("Inserire l'ultima parte della porta di arduino (escluso tty) \n-> ")    #prendiamo la seriale di arduino

    serial = Serial(port, 9600)
    serial.flushInput()

    print('- Inserire "destra" per svoltare a destra')
    print('- Inserire "sinistra" per svoltare a sinistra')
    print('- Inserire "spegni" per spegnere')
    print('- Inserire "dritto" per andare dritto')
    print('- Inserire "accendi" per accendere')
    print('- Inserire "retro" per andare in retromarcia')
    print('- Inserire un numero tra 90 e 255 per cambiare la velocita')
    print('- Inserire "ESC" per uscire\n\n')

    while True:
        toWrite = input("Cose da mandare --> ")
        if toWrite == "ESC":
            serial.close()
            sys.exit()
        else:
            serial.write(toWrite).encode())
            toRead = serial.readline()
            if toRead:
                print(toRead.decode())
