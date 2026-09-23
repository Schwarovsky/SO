#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int i;
    int j;
    pid_t pid;

    pid = fork();

    switch (pid) {

        case -1:
            printf("\nNo he podido crear el proceso hijo\n");
            break;

        case 0:
            i = 0;

            printf("\nSoy el hijo, mi PID es %d y mi variable i (inicialmente a %d) es par\n",
                   getpid(), i);

            for (j = 0; j < 5; j++) {
                i++;
                i++;

                printf("Soy el hijo, mi variable i es %d\n", i);
            }

            break;

        default:
            i = 1;

            printf("\nSoy el padre, mi PID es %d y mi variable i (inicialmente a %d) es impar\n",
                   getpid(), i);

            for (j = 0; j < 5; j++) {
                i++;
                i++;

                printf("Soy el padre, mi variable i es %d\n", i);
            }

            break;
    }

    printf("\nFinal de ejecucion de %d\n", getpid());

    exit(0);
}
