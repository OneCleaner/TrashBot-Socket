import platform
import socket
from threading import Thread
from serial import Serial
from serial import SerialException
from adafruit_servokit import ServoKit
from camera_server import camera
import RPi.GPIO as gpio
from time import sleep

MOTOR_A = 6
MOTOR_B = 12
MOTOR_TIME = 1.6

gpio.setwarnings(False)
gpio.setmode(gpio.BCM)
gpio.setup(MOTOR_A, gpio.OUT)
gpio.setup(MOTOR_B, gpio.OUT)
gpio.output(MOTOR_A, gpio.LOW)
gpio.output(MOTOR_B, gpio.LOW)

def server(indirizzo, kit, serial, backlog=1):
    try:
        skt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)    #apriamo la comunicazione tramite socket
        skt.bind(indirizzo)
        skt.listen(backlog)
        print("Server seriale inizializzato. In ascolto... ")
    except socket.error as errore:     #se esplode tutto ci si riprova
        print("Qualcosa è andato storto: \n" + str(errore))
        print("Reinizialiazzazione Server in corso...")
        server(indirizzo, kit, serial, backlog=1)
    else:   #se non esplode stabiliamo la connessione
        conn, indirizzo_client = skt.accept()  # conn = socket_client
        print("Connessione Server - Client Stabilita: " + str(indirizzo_client))

        while True:
            try:
                richiesta = conn.recv(4096)
                richiesta = richiesta.decode().split('|')

            except ConnectionResetError:        #se il client chiude forzatamente la connessione ci rimettiamo in ascolto
                print("\n\nConnessione persa, ritorno in ascolto")
                skt.close()
                server(indirizzo, kit, serial, backlog=1)

            if richiesta[0] == "ESC":  #se il client afferma di voler terminare la connessione ci rimettiamo in ascolto
                print("\n\nConnessione con Client terminata, ritorno in ascolto")
                skt.close()
                server(indirizzo, kit, serial, backlog=1)

            if richiesta[0] == "pinza-apri":
                gpio.output(MOTOR_A, gpio.HIGH)
                gpio.output(MOTOR_B, gpio.LOW)
                sleep(MOTOR_TIME)
                gpio.output(MOTOR_B, gpio.HIGH)

            if richiesta[0] == "pinza_chiudi":
                gpio.output(MOTOR_A, gpio.LOW)
                gpio.output(MOTOR_B, gpio.HIGH)
                sleep(MOTOR_TIME)
                gpio.output(MOTOR_B, gpio.HIGH)

            elif len(richiesta) == 1:
                serial.write(richiesta[0].encode())
                toRead = serial.readline()
                if toRead:
                    conn.sendall(("Comando ricevuto con successo! \n" + toRead.decode()).encode())
                else:
                    conn.sendall("Nessuna risposta dall'Arduino...".encode())  #TODO: mettere tanti try/except

            elif len(richiesta) == 2:

                angle = int(richiesta[1])
                print(richiesta)

                if richiesta[0] == "base":
                    print(richiesta)
                    kit.servo[4].angle = angle

                elif richiesta[0] == "zero":
                    print(richiesta)
                    kit.servo[3].angle = angle

                elif richiesta[0] == "rotazione-pinza":
                    print(richiesta)
                    kit.servo[12].angle = angle

                elif richiesta[0] == "non-funziona":
                    print(richiesta)
                    kit.servo[8].angle = angle

                elif richiesta[0] == "rotazione-base":
                    print(richiesta)
                    kit.servo[2].angle = angle

                conn.sendall(("Comando ricevuto: " + richiesta[0] + richiesta[1]).encode())


if __name__ == "__main__":  #eseguiamo soltanto se questa è la classe main

    while True:
        try:
            if platform.system() == "Windows":
                port = "COM" + input("Inserire il numero della porta di arduino \n-> ") #windows
            else:
                port = "/dev/tty" + input("Inserire l'ultima parte della porta di arduino (escluso tty) \n-> ")    #prendiamo la seriale di arduino
        except SerialException:
            print("Porta occupata, riprova")
            continue
        else:
            break

    serial = Serial(port, 9600)  #inizializziamo la comunicazione con arduino
    serial.flushInput()

    kit = ServoKit(channels=16)
    PORTA_SERVER = input("Inserisci una porta (Qualsiasi numero tra 1024 e 65000) \n-> ")   #prendiamo la porta su cui aprire la comunicazione con il PC
    server(("", int(PORTA_SERVER)), kit, serial, backlog=1)
    #TODO: decidere porta server fissa.
    #TODO: usare threading
    x = Thread(target=server, args=(("", int(PORTA_SERVER)), kit, serial, 1,))
    x.start()
    y = Thread(target=camera, args=(("", int(PORTA_SERVER) + 100), 1,))
    y.start()
