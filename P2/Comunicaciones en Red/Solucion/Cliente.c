// Ejercicio 1, practica 2, Sistemas Operativos
// Jose Miguel Martinez Garcia, 20523176T
// Cliente.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define ConstTam 20
#define ConstPuerto 9999


// Comprueba que se haya introducido la IP del servidor.

bool comprobarArgumentos(int argc, char *argv[]){

    if(argc != 2){
        fprintf(stderr, "Uso: %s <IP_del_servidor>\n", argv[0]);
        return false;
    }

    return true;
}


// Crea el socket que utilizara el cliente para comunicarse con el servidor.

int crearSocket(void){

    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0){
        perror("Error. No se pudo crear socket");
    }

    return sock;
}


// Configura la direccion del servidor y establece la conexion.

bool conectarServidor(int sock, char *ip){

    struct sockaddr_in serv_addr;

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_port = htons(ConstPuerto);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);

    if(connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("Error. No se pudo conectar al socket");
        close(sock);
        return false;
    }

    return true;
}


// Recibe los datos enviados por el servidor y los muestra por pantalla.

void recibirDatos(int sock){

    char buffer[ConstTam];
    ssize_t tam;

    do{

        tam = read(sock, buffer, ConstTam - 1);

        if(tam > 0){

            buffer[tam] = '\0';

            printf("%s", buffer);
        }

    }while(tam > 0);

    if(tam < 0){
        perror("error al leer del socket");
    }

}


// Funcion principal del cliente.

int main(int argc, char *argv[]){

    int sock;

    if(!comprobarArgumentos(argc, argv)){
        return 1;
    }

    sock = crearSocket();

    if(sock < 0){
        return 1;
    }

    if(conectarServidor(sock, argv[1])){

        recibirDatos(sock);

        close(sock);
    }

    return 0;
}
