// Ejercicio 3, practica 1, Sistemas Operativos
// Jose Miguel Martinez Garcia
// hijos.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>

// Funcion de vacio

void vacio(){

}

// Ejecuta el comando PSTREE para mostrar la jerarquia de procesos

void ejecutarPstree(pid_t pidHijos){

    printf("Mostrando la estructura de procesos mediante pstree...\n");

    char pidt[20];
    sprintf(pidt, "%d", pidHijos);
    execlp("pstree", "pstree", "-c", pidt, NULL);
    exit(1);
    
}

// Comprueba que los argumentos recibidos desde la terminal sean correctos

bool parse(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Uso correcto: ./hijos <x columnas> <y filas>\n");
        return false;
    }
    int x = atoi(argv[1]); /* numero de filas */
    int y = atoi(argv[2]); /* numero de columnas */

    if(x <= 0 || y <= 0){
        printf("Error: los valores introducidos deben ser enteros positivos\n");
        return false;
    }
    else{
        return true;
    }
}

// Genera la cadena vertical de procesos

int crearVertical(int func, int x, int y, int *vectorX, pid_t pidHijos){
    
    for(func = 1; func <= x; func++){

        pid_t pid = fork();

        if (getpid() == pidHijos){
            
            pid = fork();

            if(pid == 0){
                ejecutarPstree(pidHijos);
            }

            wait(NULL);
        }
        if(pid != 0){

            wait(NULL); // El proceso padre espera a que termine su hijo
            break;      // Una vez terminado, deja de crear procesos verticales y sale del bucle
        }
        else{
            // Aqui se almacena el identificador del nuevo proceso en el vector.
            vectorX[func - 1] = getpid();
            printf("Proceso %d creado. Padre: ", getpid());
            printf("%d", pidHijos);
            // Se muestran los procesos verticales creados previamente.

            /*
            En cada nueva posicion se muestran los PIDs
            de los procesos anteriores a la cadena
            */

            for(int j = 0; j < func - 1; j++){
                printf(", %d", vectorX[j]);
            }
            printf("\n");
        }
    }

    return func;
}

// Muestra por pantalla los PIDs de los procesos situados al final de cada columna

void imprimirHijosFinales(int func, int y, int *vectorY){

    printf("Proceso principal %d. Hijos finales: ", getpid());

    for(func = 0; func < y - 1; func++){

        printf("%d, ", vectorY[func]); // Se imprimen todos salvo el ultimo
    }
    printf("%d\n", vectorY[y - 1]); // El ultimo PID se muestra por separado sin coma
}

// Crea los procesos que forman la parte horizontal de la estructura

void crearHijosFinales(int func, int y, int *vectorY){

    pid_t pid;

    for(func = 1; func <= y; func++){

        pid = fork();

        if(pid == 0){

            vectorY[func - 1] = getpid(); // Guardamos el PID del proceso creado
            signal(SIGALRM, vacio);
            alarm(4);
            pause();
            exit(0);    // El hijo termina su ejecucion con exito
        }
    }
    if(func == y + 1){ // Solo el proceso padre llega hasta este punto

        for(func = 1; func <= y; func++){

            wait(NULL); // El padre espera a todos los hijos horizontales
        }
    }
}

// Funcion principal, que recibe los argumentos como parametro

int main(int argc, char *argv[])
{
    if (!parse(argc, argv)) {

        printf("Error: los argumentos introducidos no son validos.\n");
        exit(1);
    }
    else{

        int func, x, y, shmidx, shmidy;
        int *vectorX, *vectorY; // Punteros utilizados para accedes a la memoria compartida
        pid_t pidHijos;

        x = atoi(argv[1]);
        y = atoi(argv[2]);


        pidHijos = getpid();

        // Reservamos dos segmentos de memoria compartida para almacenar los PIDs.

        // Este segmento contiene los procesos de la parte vertical.
        shmidx = shmget(IPC_PRIVATE, sizeof(int) * x, IPC_CREAT | 0666);
        vectorX = (int *) shmat (shmidx, 0, 0);

        // Este otro segmento contiene los procesos de la parte horizontal.
        shmidy = shmget(IPC_PRIVATE, sizeof(int) * y, IPC_CREAT | 0666);
        vectorY = (int *) shmat (shmidy, 0, 0);

        // Contruimos la cadena vertical de procesos.
        func = crearVertical(func, x, y, vectorX, pidHijos);

        if(func == 1){ // Si func tiene valor 1, estamos en el padre principal, porque ha salido del bucle con anticipación

            imprimirHijosFinales(func, y, vectorY);

            // Desconectamos y liberamos la memoria los segmentos compartidos
            shmdt(vectorX);
            shmdt(vectorY);
            shmctl(shmidx, IPC_RMID, NULL);
            shmctl(shmidy, IPC_RMID, NULL);
        }
        if(func == x + 1){ // El ultimo proceso vertical se encarga de crear los procesos finales
            
            crearHijosFinales(func, y, vectorY);
        } 
    }

    return 0;
}