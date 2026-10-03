// Ejercicio 1 A, practica 1, Sistemas Operativos
// Jose Miguel Martinez Garcia
// malla.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>

void vacio(int n){
}

void ejecutarPstree(){

    printf("Arbol completo. Ejecuto pstree...\n");

    pid_t pidP = fork();

    if (pidP == 0) {

        char pidPadre[20];
        sprintf(pidPadre, "%d", getppid());
        execlp("pstree", "pstree", "-c", pidPadre, NULL);
        exit(1);
    }
}

bool parse(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Uso: ./malla <x filas> <y columnas>\n");
        return false;
    }
    else{

        int x = atoi(argv[1]); /* numero de filas */
        int y = atoi(argv[2]); /* numero de columnas */

        if(x <= 0 || y <= 0){
            return false;
        }
        else{
            return true;
        }
    }
    
}

void crearVertical(int x, int fila){

    for(fila = 1; fila < x; fila++){

        pid = fork();
        
        if(pid != 0){

            printf("PID = %d: he creadp a mi hijo PID = %d. Espero con wait()\n",
            getpid(), pid);

            wait(NULL);

            printf("PID = %d: mi hijo ha terminado. Yo termino\n",
            getpid());

            exit(0);
        }

        printf("PID = %d: continuo hacia la fila %d de mi columna\n",
        getpid(), fila + 1);
    }
    
    if(fila == x){

        printf("PID = %d: soy el ultimo de mi columna. Entro en pause()\n",
        getpid());

        signal(SIGALRM, vacio);
        alarm(5);
        pause();

        printf("PID = %d: ha llegado SIGALRM. Termino\n",
        getpid());
        
        exit(0);        
    }
}

void crearMalla(int x, int y, int col, int fila, pid_t pid){

    printf("P0: PID = %d, PPID = %d. Empiezo a crear columnas\n",
    getpid(), getppid());
    
    for (int col = 1; col <= y; col++) {

        pid = fork();

        if (pid == 0) {

            printf("PID = %d: soy la primera fila de la columna %d. Mi padre es %d\n",
            getpid(), col, getppid());

            crearVertical(x, fila);
        }
        else{
            printf("P0: he creado la columna %d, cuyo primer proceso es PID = %d\n",
            col, pid);
        }
    }

    if(col == y + 1){
        
        printf("P0: ya he creado todas las columnas. Espero a mis %d hijos \n",
        y);

        pid = fork();

        if(pid == 0){

            ejecutarPstree();
        }

        for(col = 1; col <= y; col++){

            printf("P0: ejecutando wait numero %d\n",
            col);

            wait(NULL);

            printf("P0: uno de mis hijos directos ha terminado, %d \n",
            col);
        }

        wait(NULL); // Espero al PSTREE

        print("P0: todos mis hijos han terminado\n");
    }
    
}

int main(int argc, char *argv[])
{
    if (!parse(argc, argv)) {
        printf("Error: los argumentos deben ser enteros positivos.\n");
        exit(1);
    }
    else{
        int x, y, col, fila;
        pid_t pid;

        x = atoi(argv[1]);
        y = atoi(argv[2]);
    
        crearMalla(x, y, col, fila, pid);
    }

    printf("Soy el programa %d y he terminado con exito", getpid());

    return 0;
}