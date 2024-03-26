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

    Carta() : valor(0.0f) {}  // default constructor

    Carta(float valor) {
        this->valor = valor;
        if (valor == 1) {
            nombre = "As";
        } else if (valor == 8) {
            nombre = "Sota";
        } else if (valor == 9) {
            nombre = "Caballo";
        } else if (valor == 10) {
            nombre = "Rey";
        } else {
            nombre = to_string(valor);
        }
    }
};

int main(int argc, char* argv[]) {  // proceso sietymedio
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <N>" << endl;
        return 1;
    }
    int N = stoi(argv[1]);
    int fd1[2];  // pipe file descriptors for the dealer pipe
    if (pipe(fd1) == -1) {
        cerr << "Error al crear el pipe." << endl;
        return 1;
    }
    int fd2[2];  // file descriptors for the player pipe
    if (pipe(fd2) == -1) {
        cerr << "Error al crear el pipe." << endl;
        return 1;
    }

    pid_t pid;

    for (int i = 0; i < N; ++i) {
        pid = fork();
        if (pid == 0) {  // fork() == 0 when it's the child process
            float puntos = 0;
            bool enJuego = true;

            int request = 1;  // request the initial card
            write(fd2[1], &request, sizeof(request));

            int cardNumber;
            read(fd1[0], &cardNumber, sizeof(cardNumber));  // read the card from the pipe

            Carta carta = Carta(cardNumber);

            cout << "Jugador " << i << " obtuvo la carta " << carta.nombre << " con valor " << carta.valor << "." << endl;
            puntos += carta.valor;

            /*
            while (enJuego) {
                int decision = rand() % 3;  // 0: pedir carta, 1: plantarse, 2: abandonar
                if (puntos > 7.5 || decision == 2) {
                    enJuego = false;
                    cout << "Jugador " << i << " ha abandonado con " << puntos << " puntos." << endl;
                } else if (decision == 1) {
                        enJuego = false;
                        cout << "Jugador " << i << " se ha plantado con " << puntos << " puntos." << endl;
                } else {
                    request = 1;  // request a card
                    write(fd2[1], &request, sizeof(request));
                    read(fd1[0], &carta, sizeof(carta));  // read the card from the pipe
                    cout << "Jugador " << i << " obtuvo la carta " << carta.nombre << " con valor " << carta.valor
                         << "." << endl;
                    puntos += carta.valor;
                }
            }*/

            cout << "---------------------------------------------------------" << endl;
            return 0;
        } else {  // parent process
            // read the request from the pipe
            int request;
            read(fd2[0], &request, sizeof(request));

            // if a card is requested, generate a card and write it to the pipe
            if (request == 1) {
                int cardNumber = rand() % 10 + 1;
                write(fd1[1], &cardNumber, sizeof(cardNumber));
            }
        }
    }
    close(fd1[1]); // close the write end of the pipe
    close(fd2[1]); // close the write end of the pipe
}