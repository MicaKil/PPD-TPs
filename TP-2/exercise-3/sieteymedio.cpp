// Created by micakil on 26/03/2024.

/*
Ejercicio Nº3 – Se deberá desarrollar una aplicación que simule partidas de siete y medio entre N jugadores. Esta aplicación estará compuesta por varios procesos, y seguirá un esquema cliente/servidor, donde los procesos clientes serán los jugadores, y el proceso servidor, se encargará de distribuir las cartas, recoger las decisiones de los jugadores, y así hasta que se declare un ganador.

 Algunas normas básicas del juego: Se juega con As, 2, 3, 4, 5, 6, 7, Sota, Caballo, y Rey. Las primeras tienen su valor numérico como puntos, y la sota, caballo, y rey tienen un valor de medio (1/2 punto). En cada ronda del repartidor, se da una carta a cada jugador (que continua en la partida), este en función del valor de sus cartas previas decide si se planta o abandona (se ha pasado de 7 y medio). La repartición de cartas continua mientras no hayan llegado todos a una situación de plante o abandono, en este caso se avalúan las cartas de los jugadores plantados, y se decide el ganador (el que más se aproxima a 7 y medio), en caso de empate se escoge a uno de ellos como ganador (por ejemplo al primero).

 La ejecución de la partida será mediante el comando: sieteymedio <N>

Donde N será el número de jugadores que participan en la primera partida. Como resultado de la ejecución se publica el ganador de la partida, así como una tabla describiendo la situación de cada jugador (cuántos puntos posee, se plantó o abandonó).

 El proceso sieteymedio será el encargado de gestionar la partida, será el que repartirá las cartas por turnos, mantendrá el estado de la partida a partir de las decisiones que le pasen los jugadores, y decidirá finalmente el ganador de la partida

 El proceso jugador será el que implementará la lógica del jugador en la partida, en base a las cartas que reciba del repartidor (sieteymedio), y de su propio estado (puntos actuales) decidirá si continúa en el juego (pide más cartas), se planta, o abandona (porque se pasó). Esta lógica de juego se deja abierta, de forma que se implemente como se quiera, pudiendo llegar a ser simplemente aleatoria: según un número aleatorio el jugador decide plantarse, pedir carta, o abandonar.

a) La generación de los N procesos jugadores deberá realizarse a través del uso de forks, y la comunicación repartidor/jugador se efectuará mediante pipes.
b) Implementar una segunda versión mediante fork y comunicación a través de sockets.
 */

#include <unistd.h>
#include <iostream>
#include <utility>

using namespace std;

class Carta {
    public:
        string nombre;
        float valor;

    Carta(string nombre, float valor) {
        this->nombre = std::move(nombre);
        this->valor = valor;
    }
};

Carta getCarta() {
    int n = rand() % 10 + 1;
    if (n == 1) {
        return {"As", 1};
    } else if (n == 8) {
        return {"Sota", 0.5};
    } else if (n == 9) {
        return {"Caballo", 0.5};
    } else if (n == 10) {
        return {"Rey", 0.5};
    } else {
        return {to_string(n), static_cast<float>(n)};
    }
}

int main(int argc, char* argv[]) {  // proceso sietymedio
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <N>" << endl;
        return 1;
    }
    int N = stoi(argv[1]);
    int fd[2];  // file descriptors for the pipe
    pipe(fd);  // create the pipe

    for (int i = 0; i < N; ++i) {
        if (fork() == 0) {  // fork() == 0 when it's the child process
            close(fd[0]);  // close the read end of the pipe
            write(fd[1], &i, sizeof(i));  // write the child process number to the pipe
            close(fd[1]);  // close the write end of the pipe

            float puntos = 0;
            bool plantado = false;
            while (!plantado) {
                Carta carta = getCarta();
                cout << "Jugador " << i << " obtuvo la carta " << carta.nombre << " con valor " << carta.valor << endl;
                puntos += carta.valor;
                cout << "Jugador " << i << " tiene " << puntos << " puntos." << endl;

                if (puntos >= 7.5) {
                    cout << "Jugador " << i << " se pasó de 7.5" << endl;
                    plantado = true;
                } else {
                    int decision = rand() % 3;
                    if (decision == 0) {
                        cout << "Jugador " << i << " se planta." << endl;
                        plantado = true;
                    } else if (decision == 1) {
                        cout << "Jugador " << i << " pide otra carta." << endl;
                    } else {
                        cout << "Jugador " << i << " abandona." << endl;
                        plantado = true;
                    }
                }
            }

            return 0;
        } else {  // parent process
            close(fd[1]);  // close the write end of the pipe
            int child_number;
            read(fd[0], &child_number, sizeof(child_number));  // read the child process number from the pipe
            cout << "Jugador " << child_number << " creado." << endl;
        }
    }
}