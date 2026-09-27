/* malla.c
 * ---------------------------------------------------------------
 * Genera el siguiente arbol de procesos:
 *
 *                    malla
 *              /   /   \      \
 *            p11  p12  p13 ... p1y
 *             |    |    |       |
 *            p21  p22  p23 ... p2y
 *             :    :    :       :
 *             |    |    |       |
 *            px1  px2  px3 ... pxy
 *
 * x = numero de filas, y = numero de columnas.
 *
 * malla crea directamente los "y" procesos de la fila 1 (uno por
 * columna). Cada uno de ellos, a su vez, crea en cadena (con un
 * unico fork por paso) al resto de procesos de su propia columna,
 * hasta llegar a la fila x.
 *
 * Para comprobar el arbol generado:
 *   $ pstree -c <pid_de_malla>
 * (o simplemente "pstree -c" si malla es el unico proceso con ese
 * nombre en el sistema)
 *
 * Todos los procesos se quedan bloqueados en pause() al terminar
 * de crear a sus descendientes, precisamente para que el arbol
 * siga vivo mientras lo inspeccionas con pstree. Para terminarlo
 * todo de golpe, ejecuta el programa en primer plano y pulsa
 * Ctrl+C (mata a todos los procesos porque comparten el mismo
 * grupo de proceso del terminal).
 *
 * Uso:
 *   gcc -o malla malla.c
 *   ./malla <x> <y>
 * ---------------------------------------------------------------
 */

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