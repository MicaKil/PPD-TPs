/*Se deberá desarrollar una aplicación que simule partidas de siete y medio entre N jugadores. Esta aplicación estará
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

#define MAX_VALUE 7.5
double TOTAL_CARDS = 40;

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
	int players_status[size - 1];
	for (player = 0; player < size; player++) {
		players_status[player] = 1;
	}

	if (rank == 0) {
		// proceso maestro
		std::cout << "Starting the game with " << size - 1 << " players." << std::endl;
		for (player = 1; player < size; player++) {
			int card = deal_cards();
			MPI_Send(&card, 1, MPI_INT, player, 0, MPI_COMM_WORLD);
		}

		int players_playing = size - 1;

		// deal cards until all players have abandoned or planted, or there are no cards left
		while (TOTAL_CARDS > 0 && players_playing > 0) {
			std::cout << "Dealing cards..." << std::endl;
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
		std::cout << "No more cards left." << std::endl;
	} else {
		// procesos trabajadores (jugadores)
		int card;
		int player_status = 1;
		while (player_status) {
			MPI_Recv(&card, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
			score += get_card_points(card);
			std::cout << "Player " << rank << " has received a " << card << " card." << std::endl;
			if (score > MAX_VALUE) {
				player_status = 0;  // abandonado
				std::cout << "Player " << rank << " has abandoned with " << score << " points." << std::endl;
				MPI_Send(&player_status, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				break;
			} else if (score == MAX_VALUE) {
				player_status = 2;  // plantado
				std::cout << "Player " << rank << " has planted with " << score << " points." << std::endl;
				MPI_Send(&player_status, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				break;
			} else {
				// gen a 0 or 1
				if (rand() % 2 == 1) {
					player_status = 1; // jugando
					std::cout << "Player " << rank << " is still playing with " << score << " points." << std::endl;
					MPI_Send(&player_status, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				} else {
					player_status = 2; // plantado
					std::cout << "Player " << rank << " has planted with " << score << " points." << std::endl;
					MPI_Send(&player_status, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
					break;
				}
			}
		}

		std::cout << "Player " << rank << " has finished with " << score << " points." << std::endl;
	}


	MPI_Barrier(MPI_COMM_WORLD);
	if (rank != 0) {
		MPI_Send(&score, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	} else {
		for (player = 1; player < size; player++) {
			MPI_Recv(&scores[player], 1, MPI_DOUBLE, player, 0, MPI_COMM_WORLD, &status);
		}
	}

	if (rank == 0) {
		double max_score = 0;
		int winner = 0;
		for (player = 1; player < size; player++) {
			if (scores[player] <= MAX_VALUE && scores[player] > max_score) {
				max_score = scores[player];
				winner = player;
			}
		}
		std::cout << "The winner is player " << winner << " with " << max_score << " points. \nCongrats! <3" << std::endl;
	}

	MPI_Finalize();
	return 0;
}