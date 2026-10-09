// Ejercicio 1 B, practica 1, Sistemas Operativos
// Jose Miguel Martinez Garcia
// ejec.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

/* Variables globales: los manejadores de señal solo reciben el
   numero de señal como parametro, asi que para que puedan usar
   otros datos (como el PID al que hay que avisar) necesitamos
   variables globales. */
pid_t pidA;              // PID de A, lo usa el manejador de Z
int senalRecibida = 0; // la usa A para saber si ya le llego la señal

// Funcion de PSTREE

void ejecutarPstree(pid_t pid){

    printf("Arbol completo. Ejecuto pstree...\n");

    char pidt[20];
    sprintf(pidt, "%d", pid);
    execlp("pstree", "pstree", "-c", pidt, NULL);
    perror("execlp");
    exit(1);
}

// Manejadores de señal

void manejadorAlarmaZ(){
    
    printf("Soy Z (%d) y muero\n", getpid());
    exit(0);
}

void manejadorUsr1A(){
    senalRecibida = 1;
}

void manejadorUsr2X(){
    printf("Soy X (%d) y muero\n", getpid());
    exit(0);
}

void manejadorUsr2Y(){
    printf("Soy Y (%d) y muero\n", getpid());
    exit(0);
}

// Proceso X
void procesoX(pid_t pidEjec, pid_t pidAbuelo, pid_t pidPadre){
    printf("Soy el proceso X: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n",
           getpid(), pidPadre, pidAbuelo, pidEjec);

    signal(SIGUSR2, manejadorUsr2X);
    pause(); /* espero a que B me avise */
}

// Proceso Y
void procesoY(pid_t pidEjec, pid_t pidAbuelo, pid_t pidPadre){
    printf("Soy el proceso Y: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n",
           getpid(), pidPadre, pidAbuelo, pidEjec);

    signal(SIGUSR2, manejadorUsr2Y);
    pause();
}

// Proceso Z
void procesoZ(pid_t pidEjec, pid_t pidAbuelo, pid_t pidPadre, int segundos){
    printf("Soy el proceso Z: mi pid es %d. Mi padre es %d. Mi abuelo es %d. Mi bisabuelo es %d\n",
           getpid(), pidPadre, pidAbuelo, pidEjec);

    pidA = pidAbuelo; /* guardo el PID de A para que lo use el manejador */
    kill(pidA, SIGUSR1);
    signal(SIGALRM, manejadorAlarmaZ);
    alarm(segundos);  
    pause();
}

// ---------------- Proceso B ----------------
void procesoB(pid_t pidEjec, pid_t pidPadre, int segundos){

    pid_t miPid = getpid();
    printf("Soy el proceso B: mi pid es %d. Mi padre es %d. Mi abuelo es %d\n",
           miPid, pidPadre, pidEjec);

    pid_t pidX = fork();
    if (pidX == 0) {
        procesoX(pidEjec, pidPadre, miPid);
    }

    pid_t pidY = fork();
    if (pidY == 0) {
        procesoY(pidEjec, pidPadre, miPid);
    }

    pid_t pidZ = fork();
    if (pidZ == 0) {
        procesoZ(pidEjec, pidPadre, miPid, segundos);
    }

    /* En este momento X e Y solo estan en pause(), nada los puede
       despertar todavia, asi que el UNICO hijo que puede morir es
       Z (cuando salte su alarma). Por eso un wait() normal ya
       reune el orden correcto sin necesidad de nada mas avanzado. */
    wait(NULL);              /* espero a que muera Z (Hijo) */

    kill(pidY, SIGUSR2);     /* ahora aviso a Y */
    wait(NULL);              /* y espero a que muera */

    kill(pidX, SIGUSR2);     /* despues aviso a X */
    wait(NULL);              /* y espero a que muera */

    printf("Soy B (%d) y muero\n", miPid);
    exit(0);
}

// ---------------- Proceso A ----------------
void procesoA(pid_t pidEjec, int segundos){
    int miPid = getpid();
    printf("Soy el proceso A: mi pid es %d. Mi padre es %d\n", miPid, pidEjec);

    signal(SIGUSR1, manejadorUsr1A);

    int pidB = fork();
    if (pidB == 0) {
        procesoB(pidEjec, miPid, segundos);
        exit(0);
    }

    while (senalRecibida == 0) {
        pause(); // espero la señal de Z
    }

    /* Creo un hijo para que ejecute pstree (asi yo puedo seguir
       vivo despues y esperar correctamente a B) */

    int pidPstree = fork();

    if (pidPstree == 0){

        ejecutarPstree(miPid);
    }
    wait(NULL); // espero a que termine de mostrarse el pstree

    wait(NULL); // espero a que B (y todo su subarbol) haya muerto

    printf("Soy A (%d) y muero\n", miPid);
    exit(0);
}

// ---------------- main = proceso ejec (arb) ----------------
int main(int argc, char *argv[]){

    if (argc != 2 || argv[1] < 0) {
        printf("Uso: ./ejec <segundos>\n");
        exit(1);
    }

    int segundos = atoi(argv[1]);
    int pidEjec = getpid();

    printf("Soy el proceso ejec: mi pid es %d\n", pidEjec);

    pid_t pidA_hijo = fork();
    if (pidA_hijo == 0) {
        procesoA(pidEjec, segundos);
        exit(0);
    }

    wait(NULL); // espero a que A (y todo el arbol) haya muerto
    printf("Soy ejec (%d) y muero\n", pidEjec);
    return 0;
}