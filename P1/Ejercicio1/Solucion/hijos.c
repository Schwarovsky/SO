//Prueba

// Ejercicio 1 B, practica 1, Sistemas Operativos
// Jose Miguel Martinez Garcia
// hijos.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>

pid_t pidHijos;

void alarma(int n){
    printf("Soy la alarma: rin rin\n");
    kill(pidHijos, SIGUSR1);
}

void morir(int n){

    printf("Soy malla: mi pid es %d y he recibido la señal de muerte\n", getpid());
}

void vacio(int n){
    
}

void ejecutarPstree(){

    printf("Arbol completo. Ejecuto pstree...\n");

    pid_t pidPs = fork();

    if (pidPs == 0) {

        char pid[20];
        sprintf(pid, "%d", pidHijos);
        execlp("pstree", "pstree", "-c", pid, NULL);
        exit(1);
    }
}

bool parse(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Uso: ./hijos <x columnas> <y filas>\n");
        return false;
    }
    int x = atoi(argv[1]); /* numero de filas */
    int y = atoi(argv[2]); /* numero de columnas */

    if(x <= 0 || y <= 0){
        return false;
    }
    else{
        return true;
    }
}

/* Crea, en cadena, los procesos de una columna. El ultimo de la
   cadena (fila == x) avisa a malla de que su columna ya esta
   completa. Todos se quedan despues en pause() esperando a que
   malla mande la orden de terminar. */
void crearHorizontal(int x, int y)
{
    int col;
    for (col = 0; col < y; col++) {
        pid_t pidHijo = fork();

        if (pidHijo == 0 && col < y - 1){

            printf("Soy p%d%d: mi pid es %d, mi padre es %d\n", x, col + 1, getpid(), getppid());
            pause(); /* AQUI tambien hace falta: que no muera nada mas crearse */
        }
    }

    signal(SIGALRM, alarma);
    alarm(2);
    pause();
    pause();
}

void crearVertical(int fila, int x, int y)
{
    printf("Soy p%d1: mi pid es %d, mi padre es %d\n", fila, getpid(), getppid());
    
    int fila;

    for(fila = 0; fila < x; fila++){
        pid_t pidHijo = fork();
        if (pidHijo > 0) {
            wait(NULL);
            exit(0);
        }
        if(pidHijo < 0){

            perror("Error en fork");
            exit(1);
        }
    }

    crearHorizontal(x, y);

    pause(); /* me quedo esperando a que me manden morir */
}

int main(int argc, char *argv[])
{
    if (!parse(argc, argv)) {
        printf("Error: los argumentos deben ser enteros positivos.\n");
        exit(1);
    }

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    pidHijos = getpid();

    printf("Soy hijos: mi pid es %d\n", pidHijos);
    signal(SIGUSR2, morir);        
    signal(SIGUSR1, vacio);
    
    crearVertical(0, x, y);
    
    pause();

     /* espero, sin gastar CPU, a que se creen todas las filas */

    ejecutarPstree();

    wait(NULL); /* espero a que pstree termine de mostrarse */

    kill(pidHijos, SIGUSR2); 

    printf("Soy malla: mi pid es %d y muero\n", getpid());

    return 0;
}