/*Se deberá desarrollar una aplicación que simule partidas de siete y medio entre N jugadores. Esta aplicación estará compuesta por varios procesos, y seguirá un esquema cliente/servidor, donde los procesos clientes serán los jugadores, y el proceso servidor, se encargará de distribuir las cartas, recoger las decisiones de los jugadores, y así hasta que se declare un ganador.
 *
 * Algunas normas básicas del juego: Se juega con As, 2, 3, 4, 5, 6, 7, Sota, Caballo, y Rey. Las primeras tienen su valor numérico como puntos, y la sota, caballo, y rey tienen un valor de medio (1/2 punto). En cada ronda del repartidor, se da una carta a cada jugador (que continúa en la partida), este en función del valor de sus cartas previas decide si se planta o abandona (se ha pasado de 7 y medio). La repartición de cartas continua mientras no hayan llegado todos a una situación de plante o abandono, en este caso se avalúan las cartas de los jugadores plantados, y se decide el ganador (el que más se aproxima a 7 y medio), en caso de empate se escoge a uno de ellos como ganador (por ejemplo al primero).
 *
 * La ejecución de la partida será mediante el comando: sieteymedio <N>
 * Donde N será el número de jugadores que participan en la primera partida. Como resultado de la ejecución se publica el ganador de la partida, así como una tabla describiendo la situación de cada jugador (cuántos puntos posee, se plantó o abandonó).
 *
 * El proceso sieteymedio será el encargado de gestionar la partida, será el que repartirá las cartas por turnos, mantendrá el estado de la partida a partir de las decisiones que le pasen los jugadores, y decidirá finalmente el ganador de la partida
 *
 * El proceso jugador será el que implementará la lógica del jugador en la partida, en base a las cartas que reciba del repartidor (sieteymedio), y de su propio estado (puntos actuales) decidirá si continúa en el juego (pide más cartas), se planta, o abandona (porque se pasó). Esta lógica de juego se deja abierta, de forma que se implemente como se quiera, pudiendo llegar a ser simplemente aleatoria: según un número aleatorio el jugador decide plantarse, pedir carta, o abandonar.
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>

#define MAX_VALUE 7.5
double TOTAL_CARDS = 40;

// función para repartir una carta aleatoria
double deal_cards() {
	if (TOTAL_CARDS <= 0) {
		return 0;
	}
	TOTAL_CARDS--;
	return (rand() % 10) + 1;
}

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

	int processes, rank, i;
	double total_points = 0;
	MPI_Status status;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes);

	//vector size of ranks -
	int ranks[processes];
	if (rank == 0) {
		// proceso maestro
		for (i = 1; i < processes; i++) {
			// repartir carta a cada jugador
			double card = deal_cards();
			MPI_Send(&card, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
		}

		double max_points = 0;
		int winner = 0;
		for (i = 1; i < processes; i++) {
			double player_points;
			MPI_Recv(&player_points, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
			if (player_points <= MAX_VALUE && player_points > max_points) {
				max_points = player_points;
				winner = i;
			}
		}
		std::cout << "El ganador es el jugador " << winner << " con " << max_points << " puntos." << std::endl;
	} else {
		// procesos trabajadores (jugadores)
		double my_points = 0;
		double card;
		while (true) {
			MPI_Recv(&card, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
			my_points += card;
			if (my_points >= MAX_VALUE || total_points >= TOTAL_CARDS - processes + 1) {
				// Enviar puntuación al proceso maestro
				MPI_Send(&my_points, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
				break;
			} else {
				// Pedir otra carta
				double new_card = deal_cards();
				MPI_Send(&new_card, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
			}
		}
	}

	MPI_Finalize();
	return 0;
}