//
// Created by micae on 21/03/2024.
//

/* Ejercicio Nº 1 – Escribir un programa en C que reciba como argumento dos números enteros, N y M. Dichos números son
utilizados para crear N threads de tipo 1 y M threads de tipo 2. La funcionalidad de los threads es la siguiente:

 THREAD tipo 1: Debe mostrar por pantalla el siguiente mensaje: “Thread1 instancia x”, siendo x el número de thread
 creado (entre 0 y N-1). Luego se suspenderá por un tiempo aleatorio entre 0 y 1 segundo (llamada usleep o sleep) y
 posteriormente incrementará la variable global “Compartida”.

 THREAD tipo 2: Debe mostrar por pantalla el siguiente mensaje: “Thread2 instancia y”, siendo y el número de thread
 creado (entre 0 y M-1). Posteriormente, se suspenderá por un tiempo aleatorio entre 0 y 1 segundo. Finalmente, leerá y
 mostrará por pantalla el valor de la variable global “Compartida”.
Cuando los threads finalicen su ejecución, el padre debe mostrar por pantalla “Fin del programa”.

 Nota: Para realizar este ejercicio deben utilizarse las llamadas pthread_create y pthread_join
 */

#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

int Compartida = 0;

void* type_1_thread(void* arg) {
    int instancia = *((int*)arg);
    cout << "Thread1 instancia " << instancia << endl;

    usleep(rand() % 1000000);

    Compartida++;

    pthread_exit(nullptr);
}

void* type_2_thread(void* arg) {
    int instancia = *((int*)arg);
    cout << "Thread2 instancia " << instancia << endl;

    usleep(rand() % 1000000);

    cout << "Valor de Compartida: " << Compartida << endl;

    pthread_exit(nullptr);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <N> <M>" << endl;
        return 1;
    }

    int N = stoi(argv[1]);
    int M = stoi(argv[2]);

    pthread_t threads_tipo_1[N];
    pthread_t threads_tipo_2[M];

    for (int i = 0; i < N; ++i) {
        int* instancia = new int(i);
		pthread_create(&threads_tipo_1[i], nullptr, type_1_thread, (void *) instancia);
    }

    for (int i = 0; i < M; ++i) {
        int* instancia = new int(i);
		pthread_create(&threads_tipo_2[i], nullptr, type_2_thread, (void *) instancia);
    }

    // wait for all threads to end
    for (int i = 0; i < N; ++i) {
        pthread_join(threads_tipo_1[i], nullptr);
    }
    for (int i = 0; i < M; ++i) {
        pthread_join(threads_tipo_2[i], nullptr);
    }

    cout << "Fin del programa" << endl;

    return 0;
}