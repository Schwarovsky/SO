// Ejercicio 2, practica 1, Sistemas Operativos
// Jose Miguel Martinez Garcia
// hacha.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define TAM_BUFFER 1000

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Uso: %s <archivo> <tamano>\n", argv[0]);
        exit(1);
    }

    char *nombreArchivo = argv[1];
    int tamFragmento = atoi(argv[2]);

    if (tamFragmento <= 0) {
        printf("El tamano debe ser un entero positivo.\n");
        exit(1);
    }

    /* Abro el archivo origen solo para lectura */
    int origen = open(nombreArchivo, O_RDONLY);
    if (origen < 0) {
        perror("Error al abrir el archivo de origen");
        exit(1);
    }

    /* Calculo el tamano total del archivo con lseek: me voy al
       final (SEEK_END) para saber la posicion, y luego vuelvo
       al principio (SEEK_SET) para empezar a leer desde ahi. */
    long tamTotal = lseek(origen, 0, SEEK_END);
    lseek(origen, 0, SEEK_SET);

    int numFragmentos = tamTotal / tamFragmento;
    if (tamTotal % tamFragmento != 0) {
        numFragmentos++;
    }

    printf("El archivo mide %ld bytes. Se generaran %d fragmentos.\n",
           tamTotal, numFragmentos);
    fflush(stdout); /* vacio el buffer de printf ANTES del primer fork():
                        si no, cada hijo hereda este mensaje sin enviar
                        todavia a pantalla, y lo volveria a imprimir el
                        solo al terminar */

    int frag;
    for (frag = 0; frag < numFragmentos; frag++) {

        int tuberia[2];
        if (pipe(tuberia) < 0) {
            perror("Error al crear la tuberia");
            exit(1);
        }

        int pid = fork();
        if (pid < 0) {
            perror("Error en fork");
            exit(1);
        }

        if (pid == 0) {
            /* ---------- PROCESO HIJO ---------- */
            close(tuberia[1]); /* el hijo solo lee de la tuberia */

            char nombreDestino[256];
            sprintf(nombreDestino, "%s.h%02d", nombreArchivo, frag);

            int destino = open(nombreDestino, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (destino < 0) {
                perror("Error al crear el archivo destino");
                exit(1);
            }

            char buffer[TAM_BUFFER];
            int leidos;
            while ((leidos = read(tuberia[0], buffer, TAM_BUFFER)) > 0) {
                write(destino, buffer, leidos);
            }

            close(destino);
            close(tuberia[0]);
            printf("Hijo: creado %s\n", nombreDestino);
            exit(0);
        }

        /* ---------- PROCESO PADRE ---------- */
        close(tuberia[0]); /* el padre solo escribe en la tuberia */

        int restante = tamFragmento;
        char buffer[TAM_BUFFER];
        while (restante > 0) {
            int aLeer = (restante < TAM_BUFFER) ? restante : TAM_BUFFER;
            int leidos = read(origen, buffer, aLeer);
            if (leidos <= 0) {
                break; /* ya no quedan mas datos en el archivo origen */
            }
            write(tuberia[1], buffer, leidos);
            restante -= leidos;
        }

        close(tuberia[1]); /* aviso al hijo de que no hay mas datos */
        wait(NULL);        /* espero a que el hijo acabe su fragmento */
    }

    close(origen);
    printf("Division completada.\n");
    
    return 0;
}