// Ejercicio 1 B, practica 1, Sistemas Operativos
// Jose Miguel Martinez Garcia
// malla.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>

pid_t pidMalla;

void alarma(int n){
    printf("Soy la alarma: rin rin");
    kill(pidMalla, SIGUSR1);
}

void morir(int n)
{
    printf("Soy malla: mi pid es %d y he recibido la señal de muerte\n", getpid());
}

void vacio(int n){
    
}

void ejecutarPstree(){

    printf("Arbol completo. Ejecuto pstree...\n");

    pid_t pidPs = fork();

    if (pidPs == 0) {

        char pid[20];
        sprintf(pid, "%d", pidMalla);
        execlp("pstree", "pstree", "-c", pid, NULL);
        exit(1);
    }
}

bool parse(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Uso: ./malla <x filas> <y columnas>\n");
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
void crearVertical(int col, int x, int y)
{
    int fila = 0;
    for(int i = 1; i < x; i++){

        pid_t pidHijo = fork();

        if(pidHijo == 0){

            printf("Soy p%d%d: mi pid es %d, mi padre es %d (fila %d, columna %d)\n",
                i + 1, col, getpid(), getppid(), i + 1, col);

        }
        else{
            //wait(NULL);
            printf("Soy %d y espero\n", getpid());
            //exit(0);
        }

        fila = i;
    }
    if(fila == x - 1 && col == y){

        signal(SIGALRM, alarma);
        alarm(2);
        pause();
        pause();
        
    }
    else{
        pause();
    }

    printf("Soy %d y muero\n", getpid());
}

void crearHorizontal(int x, int y){

    for (int col = 1; col <= y; col++) {

        pid_t pidHijo = fork();

        if (pidHijo == 0) {

            printf("Soy p%d%d: mi pid es %d, mi padre es %d (fila %d, columna %d)\n",
                1, col, getpid(), getppid(), 1, col);
            crearVertical(col, x, y);
            exit(0);
        }
        else{
            printf("Soy malla y he creado la columna %d\n", col);
        }
    }

}

int main(int argc, char *argv[])
{
    if (!parse(argc, argv)) {
        printf("Error: los argumentos deben ser enteros positivos.\n");
        exit(1);
    }

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    pidMalla = getpid();

    printf("Soy malla: mi pid es %d\n", pidMalla);
    signal(SIGUSR2, morir);        
    signal(SIGUSR1, vacio);
    
    crearHorizontal(x, y);
    
    pause();

     /* espero, sin gastar CPU, a que se creen todas las columnas */

    ejecutarPstree();

    wait(NULL); /* espero a que pstree termine de mostrarse */

    kill(pidMalla, SIGUSR2); 

    printf("Soy malla: mi pid es %d y muero\n", getpid());

    return 0;
}