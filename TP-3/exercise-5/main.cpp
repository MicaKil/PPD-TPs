/*Dada la malla de procesos que se muestra a continuación, en la que cada proceso aparece con su rango en el
 * comunicador MPI_COMM_WORLD. Escribir una única llamada a MPI_Comm_Split que permita obtener un nuevo comunicador
 * (new_comm) que incluya a los procesos que aparecen con fondo gris en la figura, pero cuyos rangos se ordenen de forma
 * inversamente proporcional al rango que tenían en el comunicador original. Utilizar el menor número de órdenes posible.
 *
 * a) 0  1  2  3 [gris]
 *    4  5  6  7
 *    8  9  10 11 [gris]
 *    12 13 14 15
 *
 * b) 0 [gris] 1         2 [gris]  3
 *    4        5 [gris]  6         7 [gris]
 *    8 [gris] 9         10 [gris] 11
 *    12       13 [gris] 14        15 [gris]
 */

#include <mpi.h>
#include <iostream>

int main(int argc, char *argv[]) {
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		std::cerr << "Error al inicializar MPI." << std::endl;
		return 1;
	}

	int size, rank, new_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// check if size is multiple of 4
	if (size != 16) {
		if (rank == 0) {
			std::cerr << "Error: Se requieren 16 procesos para este ejercicio." << std::endl;
		}
		return 1;
	}

	if (argc <= 1) {
		if (rank == 0) {
			std::cerr << "Error: Falta argumento. Debe ser 'a' o 'b'." << std::endl;
		}
		return 1;
	}

	char exercise;
	// check if an argument was given
	if (argc > 1) {
		// parse the argument
		exercise = argv[1][0];
		if (exercise != 'a' && exercise != 'b') {
			if (rank == 0) {
				std::cerr << "Error: Argumento inválido. Debe ser 'a' o 'b'." << std::endl;
			}
			return 1;
		}
	}

	int color = 1;  // white
	int key = rank;
	if (exercise == 'a') {
		if (rank < 4 || (rank >= 8 && rank < 12)) {
			color = 0;  // grey
			key = - rank;
		}
	} else {
		if (rank < 4 || (rank >= 8 && rank < 12)) {
			if (rank % 2 == 0) {
				color = 0;  // grey
				key = - rank;
			}
		} else {
			if (rank % 2 != 0) {
				color = 0;  // grey
				key = - rank;
			}
		}
	}

	MPI_Comm new_comm;  //create new communicator

	// split the communicator in two groups, one for gray and one for white
	MPI_Comm_split(MPI_COMM_WORLD, color, key, &new_comm);
	MPI_Comm_rank(new_comm, &new_rank); //get the new rank

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Barrier(new_comm);

	std::cout << "Rank original: " << rank << ". Color: " << (color == 0 ? "Gris" : "Blanco") << ". Nuevo rank: " << new_rank << std::endl;

	MPI_Finalize();
	return 0;
}