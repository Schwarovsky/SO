#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    int estado, numero;

    switch (fork())
    {
        case -1:
            perror("Error en fork");
            exit(1);

        case 0:
            numero = 13;
            printf("Soy el hijo y muero con %d...\n", numero);
            sleep(20);
            exit(numero);

        default:
            wait(&estado);
            printf("Soy el padre. ");

            if ((estado & 0x7F) != 0)
            {
                printf("Mi hijo ha muerto con una señal.\n");
            }
            else
            {
                printf("Mi hijo ha muerto con exit(%d).\n",
                       (estado >> 8) & 0xFF);
            }
    }

    exit(0);
}