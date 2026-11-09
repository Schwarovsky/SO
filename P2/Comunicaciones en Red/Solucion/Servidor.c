// Ejercicio 1, practica 2, Sistemas Operativos
// Jose Miguel Martinez Garcia, 20523176T
// Servidor.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define ConstTam 20
#define ConstPuerto 9999


// Crea y configura el socket del servidor.

int crearServidor(){

    struct sockaddr_in serverAddress;
    int sock;
    int opcion = 1;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0){
        perror("Error. No se pudo abrir el socket");
        return -1;
    }

    bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_port = htons(ConstPuerto);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion));

    if(bind(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
        perror("Error no se pudo vincular el socket");
        close(sock);
        return -1;
    }

    if(listen(sock, 5) < 0){
        perror("error en listen");
        close(sock);
        return -1;
    }

    return sock;
}


// Lee el fichero y transmite su contenido al cliente.

void transmitirFichero(int newSock){

    ssize_t leidos, enviados, escritos;
    int fichOrigen;
    char buffer[ConstTam];

    fichOrigen = open("Google.html", O_RDONLY);

    if(fichOrigen < 0){
        perror("error al abrir el fichero");
    }
    else{

        // TRANSMISION DEL FICHERO.

        leidos = read(fichOrigen, buffer, ConstTam - 1);

        while(leidos > 0){

            printf("he leido del fichero un bloque de %zd\n", leidos);

            enviados = 0;

            while(enviados < leidos){

                escritos = write(newSock, buffer + enviados, leidos - enviados);

                if(escritos <= 0){
                    perror("error al enviar al cliente");
                    break;
                }

                enviados += escritos;
            }

            if(enviados < leidos) break;

            leidos = read(fichOrigen, buffer, ConstTam - 1);
        }

        if(leidos < 0) perror("error al leer el fichero");

        printf("ya he acabado, tamaño del ultimo bloque leido %zd\n", leidos);

        close(fichOrigen);
    }
}


// Atiende la petición de un cliente desde el proceso hijo.

void atenderCliente(int sock, int newSock){

    close(sock); // El hijo no acepta nuevas conexiones.

    printf("hijo acepta conexion\n");

    transmitirFichero(newSock);

    printf("soy el que atendia la peticion, ya he acabado\n");

    close(newSock);

    fflush(stdout);

    exit(0); // El hijo no vuelve a accept.
}


// Espera conexiones y crea un hijo para atender cada cliente.

void esperarClientes(int sock){

    socklen_t tam;
    struct sockaddr_in clientAddress;
    int newSock;
    pid_t pid;

    signal(SIGCHLD, SIG_IGN); // Evita hijos zombis.
    signal(SIGPIPE, SIG_IGN); // Evita que un cliente desconectado termine el proceso.

    do{

        tam = sizeof(clientAddress);

        newSock = accept(sock, (struct sockaddr *) &clientAddress, &tam);

        if(newSock < 0){

            perror("error al aceptar la conexion");
        }
        else{

            printf("aceptando conexion\n");
            fflush(stdout);

            pid = fork();

            if(pid == 0){

                atenderCliente(sock, newSock);
            }
            else if(pid > 0){

                close(newSock); // El padre no transmite el fichero.
            }
            else{

                perror("error en fork");
                close(newSock);
            }
        }

    }while(1);
}


int main(){

    int sock;

    sock = crearServidor();

    if(sock < 0){

        return 1;
    }

    esperarClientes(sock);

    return 0;
}
