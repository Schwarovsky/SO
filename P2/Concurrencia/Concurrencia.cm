// Ejercicio 2, practica 2, Sistemas Operativos
// Jose Miguel Martinez Garcia, 20523176T
// Concurrencia.cm

#include "gdefs.cm"

const
    int VARIABLE1 = 1;
    int VARIABLE2 = 2;
    int BARRIER1 = 4;
    int BARRIER2 = 5;
    int DELTA = 11;
    int Xbarrier1 = 200;
    int Xbarrier2 = 400;
    int Xfinal = 600;

// Semaforo que controla los espacios libres de la seccion critica.
// El productor espera cuando no quedan huecos disponibles.
// Los robots incrementan este semaforo cuando retiran una pieza.
semaphore huecos = 5;

// Semaforo que controla las piezas disponibles en la seccion critica.
// Los consumidores esperan cuando no hay piezas para recoger.
// El rpoductor incrementa este semaforo cuando deposita una pieza.
semaphore paquetes = 0;

// Semaforo que garantiza la exclusion mutua.
// Permite que solamente un proceso acceda a la seccion critica
// en cada momento, evitando modificaciones simultaneas.
semaphore mutex = 1;

// Identificador utilizado para gestionar las piezas de la seccion critica.
int idPieza = 30;

// ------------------ PRODUCTOR ------------------

void productor(){
    int i;

    // El productor fabrica un total de 100 piezas.
    for(i = 1; i <= 200; i++){

        cout << "PRODUCTOR: Produciendo pieza " << i << endl;

        // Comprueba que exista espacio libre en la seccion critica.
        wait(huecos);

        // Solicita acceso exclusivo a la seccion critica.
        wait(mutex);

        // Crea un paquete circular de color verde y tamaño 80x80.
        create(idPieza, CIRCLE, GREEN, 220, 20 + (idPieza - 29) * 15, 80, 80);

        idPieza++;

        // El productor avanza para depositar la pieza.
        moveby(VARIABLE1, 200, 0);

        cout << "PRODUCTOR: dejando paquete " << i << endl;

        // Libera el acceso a la seccion critica.
        signal(mutex);

        // El productor vuelve a su posicion inicial.
        moveby(VARIABLE1, -200, 0);

        // Notifica que hay una nueva pieza disponible.
        signal(paquetes);
    }
}

// ------------------ CONSUMIDOR ------------------

void consumidor(){
    int i;

    // Cada consumidor recoge un total de 100 piezas.
    for(i = 1; i <= 100; i++){

        cout << "CONSUMIDOR: buscando paquete " << i << endl;

        // Espera hasta que haya una pieza disponible.
        wait(paquetes);

        // Solicita acceso exclusivo a la seccion critica.
        wait(mutex);

        // El consumidor se desplaza hacia la seccion critica.
        moveby(VARIABLE2, -200, 0);

        cout << "CONSUMIDOR: cogiendo paquete " << i << endl;

        // Actualiza el identificador de la pieza.
        idPieza--;

        // Desplaza la pieza hacia la caja.
        moveby(idPieza, 700, 0);

        // El consumidor vuelve a su posicion inicial.
        moveby(VARIABLE2, 200, 0);

        // Libera el acceso a la seccion critica.
        signal(mutex);

        // Notifica que hay un nuevo espacio disponible.
        signal(huecos);

        cout << "CONSUMIDOR: dejando paquete en caja" << endl;
    }
}

// ------------------ PROGRAMA PRINCIPAL ------------------

void main(){

    // Productor representado mediante un triangulo rojo.
    create(VARIABLE1, TRIANGLE, RED, 30, 100, 80, 80);

    // Consumidores representados mediante triangulos azules.
    create(VARIABLE2, TRIANGLE, BLUE, 700, 100, 80, 80);

    // Barreras que delimitan la zona de trabajo.
    create(BARRIER1, RECTANGLE, GREEN, 200, 100, 5, 800);
    create(BARRIER2, RECTANGLE, GREEN, 600, 100, 5, 800);

    // Ejecucion concurrente del productor y los dos consumidores.
    cobegin{
        productor();
        consumidor();
        consumidor();
    }
}