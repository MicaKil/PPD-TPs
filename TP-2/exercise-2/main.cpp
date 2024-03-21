/*
Ejercicio Nº1 – Escribir un programa en C que reciba como argumento dos números enteros, N y M. Dichos números son utilizados para crear N threads de tipo 1 y M threads de tipo 2. La funcionalidad de los threads es la siguiente:

THREAD tipo 1: Debe mostrar por pantalla el siguiente mensaje: “Thread1 instancia x”, siendo x el número de thread creado (entre 0 y N-1). Luego se suspenderá por un tiempo aleatorio entre 0 y 1 segundo (llamada usleep o sleep) y posteriormente incrementará la variable global “Compartida”.

 THREAD tipo 2: Debe mostrar por pantalla el siguiente mensaje: “Proceso 2 instancia y”, siendo y el número de thread creado (entre 0 y M-1). Posteriormente se suspenderá por un tiempo aleatorio entre 0 y 1 segundo. Finalmente leerá y mostrará por pantalla el valor de la variable global “Compartida”.

 Cuando los threads finalicen su ejecución, el padre debe mostrar por pantalla “Fin del programa”.

 Nota: Para realizar este ejercicio deben utilizarse las llamadas pthread_create y pthread_join

 Ejercicio Nº2 – Realizar el ejercicio anterior, pero esta vez utilizando procesos en vez de threads. Es decir, ahora se deberán crear, por medio de la llamada fork, N PROCESOS tipo 1 y M PROCESOS tipo 2. ¿Qué comportamiento puede observarse? ¿Qué diferencia existe entre esta implementación y la del ejercicio anterior?

*/

#include <unistd.h>
#include <iostream>
#include <wait.h>


using namespace std;
int Compartida = 0;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <N> <M>" << endl;
        return 1;
    }

    int N = stoi(argv[1]);
    int M = stoi(argv[2]);

    for (int i = 0; i < N; ++i) {
        if (fork() == 0) {  // fork() == 0 when it's the child process
            cout << "Proceso 1 instancia " << i << endl;
            usleep(rand() % 1000000);
            Compartida++;
            return 0;
        }
    }

    for (int i = 0; i < M; ++i) {
        if (fork() == 0) {
            cout << "Proceso 2 instancia " << i << endl;
            usleep(rand() % 1000000);
            cout << "Valor de Compartida: " << Compartida << endl;
            return 0;
        }
    }

    for (int i = 0; i < N + M; ++i) {
        wait(nullptr);  // wait for any child process
    }

    cout << "Fin del programa" << endl;

    return 0;
}

// With processes the global variable Compartida is not shared between the processes, so the value of Compartida is always 0 for the type 2 processes.

