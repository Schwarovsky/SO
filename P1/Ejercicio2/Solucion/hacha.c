// Ejercicio 2, practica 1, Sistemas Operativos
// Jose Miguel Martinez Garcia
// hacha.c

#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

// Calcula cuantos trozos hacen falta: la division entera del tamaño total
// entre el tamaño del trozo, mas uno si sobran bytes (el ultimo trozo,
// que sera mas pequeño que los demas).
int calcularNumTrozos(long tamTotal, int tamanyo){

    int numTrozos = tamTotal / tamanyo;

    if(tamTotal % tamanyo != 0){
        numTrozos++;
    }
    return numTrozos;
}

// Trabajo del HIJO: lee del tubo todo lo que le manda el padre y lo guarda
// en su propio archivo (nombre.hNN).
void escribirTrozo(int lectura, int escritura, char *nombre, int i, int tamanyo, char *buffer){

    // El hijo solo lee del tubo, asi que cierra su copia del extremo de
    // escritura. Si no lo cerrara, read() nunca devolveria 0 (fin de datos),
    // porque el propio hijo seguiria contando como "escritor" del tubo.
    close(escritura);

    char nombreFichero[256];
    sprintf(nombreFichero, "%s.h%d", nombre, i + 1);
    int fSalida = creat(nombreFichero, 0666);

    // Un tubo puede entregar los datos en varias partes, asi que se repite
    // read() hasta que devuelva 0: eso ocurre cuando el tubo esta vacio y el
    // padre ya ha cerrado su extremo de escritura.
    int numLeidos;
    while((numLeidos = read(lectura, buffer, tamanyo)) > 0){
        write(fSalida, buffer, numLeidos);
    }

    close(fSalida);
    close(lectura);
}

// Trabajo del PADRE para un trozo: lee "tamanyo" bytes del archivo original
// y los mete en el tubo para que los recoja el hijo.
void enviarTrozo(int fEntrada, int lectura, int escritura, char *buffer, int tamanyo){

    // El padre solo escribe en este tubo, asi que cierra el extremo de lectura.
    close(lectura);

    int numLeidos = read(fEntrada, buffer, tamanyo);
    write(escritura, buffer, numLeidos);

    // Al cerrar el extremo de escritura el hijo sabe que no llegaran mas datos
    // y su bucle de lectura termina.
    close(escritura);
}

// Parte el archivo "nombre" en trozos de "tamanyo" bytes, creando un hijo por
// cada trozo. Cada trozo usa un tubo distinto.
void partir(char *nombre, int tamanyo){

    int fEntrada = open(nombre, O_RDONLY);

    if(fEntrada < 0){

        printf("Error. No existe el fichero\n");
    }
    else{

        // stat rellena propFichero con la informacion del archivo; aqui solo se
        // usa su tamaño total en bytes (st_size).
        struct stat propFichero;
        stat(nombre, &propFichero);

        int numTrozos = calcularNumTrozos(propFichero.st_size, tamanyo);

        // Memoria donde se guarda cada trozo mientras pasa por el tubo.
        // Tras el fork, padre e hijo tienen cada uno su propia copia.
        char *buffer = (char *) malloc(sizeof(char) * tamanyo);

        int tuberia[2]; // tuberia[0] = lectura, tuberia[1] = escritura

        for(int i = 0; i < numTrozos; i++){

            pipe(tuberia);

            if(fork() == 0){
                // Proceso hijo: escribe su trozo y termina aqui, para que no
                // siga ejecutando el bucle y creando mas hijos.
                escribirTrozo(tuberia[0], tuberia[1], nombre, i, tamanyo, buffer);
                free(buffer);
                exit(0);
            }

            // Proceso padre: manda el trozo y pasa a crear el siguiente hijo.
            enviarTrozo(fEntrada, tuberia[0], tuberia[1], buffer, tamanyo);
        }

        // El padre espera a que terminen todos los hijos antes de acabar.
        for(int i = 0; i < numTrozos; i++){
            wait(NULL);
        }

        close(fEntrada);
        free(buffer);   
    }
}

// ---------- Funcion principal ------------

int main(int argc, char *argv[]){

    if(argc != 3){
        printf("Error. Uso: %s fichero tam_trozo\n", argv[0]);
    }
    else{
        int tamanyo = atoi(argv[2]);

        // Si tamanyo fuera 0 o negativo, calcularNumTrozos dividiria entre 0.
        if(tamanyo <= 0){
            printf("Error. tam_trozo debe ser un entero positivo\n");
        }
        else{
            partir(argv[1], tamanyo);
        }
    }
    return 0;
}