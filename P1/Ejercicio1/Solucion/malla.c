// Ejercicio 1 A, practica 1, Sistemas Operativos
// Jose Miguel Martinez Garcia
// malla.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Crea, en cadena, los procesos de una columna: primero se
   presenta el propio proceso (fila, columna) y despues, si
   todavia no hemos llegado a la ultima fila, crea a su hijo
   (fila+1, columna) mediante un unico fork. */
void crearFila(int fila, int col, int x)
{
    printf("Soy p%d%d: mi pid es %d, mi padre es %d (fila %d, columna %d)\n",
           fila, col, getpid(), getppid(), fila, col);

    if (fila < x) {
        int pidHijo = fork();
        if (pidHijo == 0) {
            crearFila(fila + 1, col, x);
            exit(0);
        }
    }

    pause(); /* me quedo vivo para que se me vea con pstree -c */
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Uso: %s <x filas> <y columnas>\n", argv[0]);
        exit(1);
    }

    int x = atoi(argv[1]); /* numero de filas */
    int y = atoi(argv[2]); /* numero de columnas */

    printf("Soy malla: mi pid es %d\n", getpid());

    int col;
    for (col = 1; col <= y; col++) {
        int pidHijo = fork();
        if (pidHijo == 0) {
            crearFila(1, col, x);
            exit(0);
        }
    }

    pause(); /* malla tambien se queda viva, es la raiz del arbol */
    return 0;
}