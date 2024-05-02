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
 * b) 0 [gris] 1  2 [gris] 3
 *    4  5 [gris] 6  7 [gris]
 *    8 [gris] 9  10 [gris] 11
 *    12 13 [gris] 14 15 [gris]*/

#include <mpi.h>
#include <vector>
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
	if (size / 16 != 1) {
		if (rank == 0) {
			std::cerr << "Error: Se requieren 16 procesos para este ejercicio." << std::endl;
		}
		MPI_Finalize();
		return 0;
	}

	// check if an argument was given
	if (argc <= 1) {
		if (rank == 0)
			std::cerr << "Error: Se requiere un argumento [a/b] para este ejercicio." << std::endl;
		MPI_Finalize();
		return 1;
	}

	char exercise;
	// parse the argument
	exercise = argv[1][0];
	if (exercise != 'a' && exercise != 'b') {
		if (rank == 0)
			std::cerr << "Error: Argumento inválido. Debe ser 'a' o 'b'." << std::endl;
		MPI_Finalize();
		return 1;
	}




	//assign gray color to the processes in even rows
	int color, key;

	if (exercise == 'a')
		if (rank < 4 || (rank >= 8 && rank < 12)) {
			color = 0;  // gray
			key = -rank;
		}
		else {
			color = 1;
			key = rank;
		}
	else
		if (rank < 4 || (rank >= 8 && rank < 12))
			color = 0;  // gray
		else
			color = 1;
		// get even number in first row, odd in second, and so on

	//create new communicator
	MPI_Comm new_comm;
	//split the communicator
	MPI_Comm_split(MPI_COMM_WORLD, color, key, &new_comm);  // split the communicator in two groups, one for gray and one for white
	//get the new rank
	MPI_Comm_rank(new_comm, &new_rank);

	std::vector<int> results(size);
	MPI_Gather(&new_rank, 1, MPI_INT, results.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	std::cout << "Rank original: " << rank << ". Color: " << ((color == 0) ? "Gris" : "Blanco") << ". Nuevo rank: " << new_rank << std::endl;

	if (rank == 0) {
		std::cout << "Resultados:" << std::endl;
		for (int i = 0; i < size; ++i) {
			std::cout << "Rank original: " << i << ". Color: " << ((color == 0) ? "Gris" : "Blanco") << ". Nuevo rank: " << results[i] << std::endl;
		}
		std::cout << std::endl;
	}

	MPI_Finalize();
	return 0;
}