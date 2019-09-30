# Questo programma va eseguito sul PC

import socket
import sys


def invia_comandi(skt):
    print('- Inserire "destra" per svoltare a destra')
    print('- Inserire "sinistra" per svoltare a sinistra')
    print('- Inserire "spegni" per spegnere')
    print('- Inserire "accendi" per accendere')
    print('- Inserire un numero tra 90 e 255 per cambiare la velocità')
    print('- Inserire "ESC" per uscire\n\n')
    while True:
        comando = input("-> ")
        if comando == "ESC":
            print("Sto chiudendo la connessione con il Raspberry")
            skt.send(comando.encode())
            skt.shutdown(socket.SHUT_RDWR) #solo client
            skt.close()
            sys.exit()
        else:
            skt.send(comando.encode())
            data = skt.recv(4096)
            print(str(data, "utf-8"))


def connessione_server(indirizzo_server):
    try:
        skt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        skt.connect(indirizzo_server)
    except socket.error as errore:
        print("Connessione Fallita: \n" + str(errore))
        skt.shutdown(socket.SHUT_RDWR)
        skt.close()
        sys.exit()
    else:
        print("Connessione al Raspberry Riuscita!")
        invia_comandi(skt)


if __name__ == "__main__":

    IP_RASPBERRY = input("Inserisci l'indirizzo del Raspberry \n-> ")
    PORTA_RASPBERRY = input("Inserisci la porta che hai inserito nel Raspberry \n-> ")
    connessione_server((IP_RASPBERRY, int(PORTA_RASPBERRY)))
