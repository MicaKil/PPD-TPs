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
		std::cerr << "Error al inicializar MPI" << std::endl;
		return 1;
	}

	int size, rank, new_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// check if size is multiple of 4
	if (size % 4 != 0) {
		if (rank == 0) {
			std::cerr << "Error: Se requiere un múltiplo de 4 para este ejercicio." << std::endl;
		}
		MPI_Finalize();
		return 0;
	}

	//assign gray color to the processes in even rows
	int color = (rank / 4) % 2 ? 1 : 0; // 0 for gray, 1 for white

	//create new communicator
	MPI_Comm new_comm;
	//split the communicator
	MPI_Comm_split(MPI_COMM_WORLD, color, -rank, &new_comm);  // split the communicator in two groups, one for gray and one for white
	//get the new rank
	MPI_Comm_rank(new_comm, &new_rank);

	std::vector<int> results(size);
	MPI_Gather(&new_rank, 1, MPI_INT, results.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0) {
        for (int i = 0; i < size; ++i) {
            std::cout << "Rank original: " << i << ". Color: " << ((color == 0) ? "Blanco" : "Gris") << ". Nuevo rank: " << results[i] << std::endl;
        }
        std::cout << std::endl;
    }

	MPI_Finalize();
	return 0;
}