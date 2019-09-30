from time import sleep
from picamera import PiCamera
import socket

def camera(indirizzo, backlog=1):
    camera = PiCamera()
    camera.capture("image.png")
    try:
        camera_skt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        camera_skt.bind(indirizzo)
        camera_skt.listen(backlog)
        print("Server camera inizializzato. In ascolto... ")
    except socket.error as errore:  # se esplode tutto ci si riprova
        print("Qualcosa è andato storto: \n" + str(errore))
        print("Reinizialiazzazione Server in corso...")
        camera(indirizzo, backlog=1)
    else:  # se non esplode stabiliamo la connessione
        conn, indirizzo_client = camera_skt.accept()  # conn = socket_client
        print("Connessione Server - Client Stabilita: " + str(indirizzo_client))

        while True:
            camera.capture("image.png")
            file = open("image.png", "rb")
            conn.send(file.read())
            file.close()
            sleep(10)