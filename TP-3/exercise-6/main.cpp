/* Programación Paralela y Distribuida, Lic. en Ciencias de la Computación, Facultad de Ingeniería, Universidad Nacional de Cuyo
 * Del Longo, Micaela
 * 8 de mayo de 2024
 *
 * Para compilar se utiliza cmake, para ello se deben seguir los siguientes pasos:
 * 1. En la carpeta raíz del proyecto, crear una carpeta donde se almacenarán los archivos generados por cmake: mkdir <build>
 * 2. Moverse a la carpeta creada: cd <build>
 * 3. Ejecutar cmake: cmake ..
 * 4. Compilar el proyecto: cmake --build .
 *
 * Para este ejercicio se requieren al menos tres procesos, uno para el proceso maestro y dos para los procesos trabajadores.
 *
 * Para ejecutar el programa dentro de la carpeta <build> se debe ejecutar el siguiente comando:
 * mpirun -n <Núm. de procesos> --oversubscribe TP3exercise6
 *
 * Ejercicio 6:
 *
 * Se deberá desarrollar una aplicación que simule partidas de siete y medio entre N jugadores. Esta aplicación estará
 * compuesta por varios procesos, y seguirá un esquema cliente/servidor, donde los procesos clientes serán los
 * jugadores, y el proceso servidor, se encargará de distribuir las cartas, recoger las decisiones de los jugadores,
 * y así hasta que se declare un ganador.
 *
 * Algunas normas básicas del juego: Se juega con As, 2, 3, 4, 5, 6, 7, Sota, Caballo, y Rey. Las primeras tienen su
 * valor numérico como puntos, y la sota, caballo, y rey tienen un valor de medio (1/2 punto). En cada ronda del
 * repartidor, se da una carta a cada jugador (que continúa en la partida), este en función del valor de sus cartas
 * previas decide si se planta o abandona (se ha pasado de 7 y medio). La repartición de cartas continua mientras
 * no hayan llegado todos a una situación de plante o abandono, en este caso se avalúan las cartas de los jugadores
 * plantados, y se decide el ganador (el que más se aproxima a 7 y medio), en caso de empate se escoge a uno de ellos
 * como ganador (por ejemplo al primero).
 *
 * La ejecución de la partida será mediante el comando: sieteymedio <N>
 * Donde N será el número de jugadores que participan en la primera partida. Como resultado de la ejecución se publica
 * el ganador de la partida, así como una tabla describiendo la situación de cada jugador (cuántos puntos posee,
 * se plantó o abandonó).
 *
 * El proceso sieteymedio será el encargado de gestionar la partida, será el que repartirá las cartas por turnos,
 * mantendrá el estado de la partida a partir de las decisiones que le pasen los jugadores, y decidirá finalmente
 * el ganador de la partida
 *
 * El proceso jugador será el que implementará la lógica del jugador en la partida, en base a las cartas que reciba
 * del repartidor (sieteymedio), y de su propio estado (puntos actuales) decidirá si continúa en el juego (pide más
 * cartas), se planta, o abandona (porque se pasó). Esta lógica de juego se deja abierta, de forma que se implemente
 * como se quiera, pudiendo llegar a ser simplemente aleatoria: según un número aleatorio el jugador decide plantarse,
 * pedir carta, o abandonar.
 */

#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <algorithm>
#include <string>

#define MAX_VALUE 7.5
double TOTAL_CARDS = 40;

void calculate_results(int size, int player, const double *scores, const int *players_status);

// generate a random number between 1 and 10
int deal_cards() {
	if (TOTAL_CARDS <= 0) {
		return 0;
	}
	TOTAL_CARDS -= 1;
	return (rand() % 10) + 1;
}

// function to get the points of a card
double get_card_points(int card) {
	if (card <= 7) {
		return card;
	} else if (card <= 10) {
		return 0.5;
	} else {
		return 0;
	}
}

std::string get_card_name(int card) {
	switch (card) {
		case 1:
			return "As";
		case 2:
			return "2";
		case 3:
			return "3";
		case 4:
			return "4";
		case 5:
			return "5";
		case 6:
			return "6";
		case 7:
			return "7";
		case 8:
			return "Sota";
		case 9:
			return "Caballo";
		case 10:
			return "Rey";
		default:
			return "0";  // no debería llegar acá -> se deja por prolijidad
	}
}

// shows the results of the game and calculates the winner
void calculate_results(int size, int player, const double *scores, const int *players_status) {
	std::cout << "\nPartida finalizada. Resultados:\n" << std::endl;
	double max_score = 0;
	int winner = 0;
	for (player = 1; player < size; player++) {
		switch (players_status[player]) {
			case 0:
				std::cout << "El jugador " << player << " abandonó con " << scores[player] << " puntos." << std::endl;
				break;
			case 1:
				std::cout << "El jugador " << player << " finalizó con " << scores[player] << " puntos." << std::endl;
				break;
			case 2:
				std::cout << "El jugador " << player << " se plantó con " << scores[player] << " puntos." << std::endl;
				break;
		}
		if (scores[player] <= MAX_VALUE && scores[player] > max_score) {
			max_score = scores[player];
			winner = player;
		}
	}
	std::cout << "\n¡El ganador es el jugador " << winner << " con " << max_score << " puntos!" << std::endl;
}


int main(int argc, char **argv) {
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		std::cerr << "Error al inicializar MPI." << std::endl;
		return 1;
	}

	int size, rank, player;
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	srand(rank + time(NULL));

	double score = 0;
	double scores[size - 1];  // array to store the scores of the players
	for (player = 0; player < size; player++) {
		scores[player] = 0;
	}

	// player status
	// 0: abandonado
	// 1: jugando
	// 2: plantado
	// 3: se quedó sin cartas
	int players_status[size - 1];
	for (player = 0; player < size; player++) {
		players_status[player] = 1;
	}

	if (rank == 0) {
		// proceso maestro
		std::cout << "¡Empieza la partida de 7 y medio con " << size << " jugadores y " << TOTAL_CARDS << " cartas!\n" << std::endl;
		for (player = 1; player < size; player++) {
			int card = deal_cards();
			MPI_Send(&card, 1, MPI_INT, player, 0, MPI_COMM_WORLD);
		}

		int players_playing = size - 1;

		// deal cards until all players have abandoned or planted, or there are no cards left
		while (TOTAL_CARDS > 0 && players_playing > 0) {
			int player_status;
			MPI_Recv(&player_status, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			players_status[status.MPI_SOURCE] = (int) player_status;
			if (player_status == 1) {
				int card = deal_cards();
				MPI_Send(&card, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			} else {
				players_playing -= 1;
			}
		}

	} else {
		// workers (players)
		int card;
		int player_status = 1;
		while (player_status) {
			MPI_Recv(&card, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
			if (card == 0) {
				player_status = 3;  // se quedó sin cartas
				std::cout << "El jugador " << rank << " se quedó sin cartas." << std::endl;
				MPI_Send(&player_status, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				break;
			} else {
				score += get_card_points(card);
				std::cout << "El jugador " << rank << " ha recibido la carta " << get_card_name(card) << " con " << get_card_points(card) << " puntos." << std::endl;
				if (score > MAX_VALUE) {
					player_status = 0;  // abandonado
					std::cout << "El jugador " << rank << " ha abandonado con " << score << " puntos." << std::endl;
					MPI_Send(&player_status, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
					break;
				} else if (score == MAX_VALUE) {
					player_status = 2;  // plantado
					std::cout << "El jugador " << rank << " se ha plantado con " << score << " puntos." << std::endl;
					MPI_Send(&player_status, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
					break;
				} else {
					// gen a 0 or 1
					if (rand() % 2 == 1) {
						player_status = 1; // jugando
						std::cout << "El jugador " << rank << " sigue jugando con " << score << " puntos." << std::endl;
						MPI_Send(&player_status, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
					} else {
						player_status = 2; // plantado
						std::cout << "El jugador " << rank << " se ha plantado con " << score << " puntos." << std::endl;
						MPI_Send(&player_status, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
						break;
					}
				}
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank != 0) {  // workers (players) send their scores to the master
		MPI_Send(&score, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	} else {
		for (player = 1; player < size; player++) {
			// master receives them and stores them in the scores array
			MPI_Recv(&scores[player], 1, MPI_DOUBLE, player, 0, MPI_COMM_WORLD, &status);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0) {
		// master calculates the winner
		calculate_results(size, player, scores, players_status);
	}

	MPI_Finalize();
	return 0;
}
