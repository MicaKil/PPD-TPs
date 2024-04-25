/*Escribir un programa en MPI que implemente un “ping-pong”, esto es, un programa en el que un proceso envía un mensaje
 * a otro y este último lo devuelve inmediatamente al primero. Utilizar la función MPI_Wtime para calcular cuánto
 * tiempo se invierte en esta operación. */

#include <mpi.h>

int main(int argc, char *argv[]) {
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		std::cerr << "Error al inicializar MPI" << std::endl;
		return 1;
	}

	double initial_time, final_time;

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (size != 2) {
		if (rank == 0)
			std::cerr << "Se necesitan exactamente 2 procesos para este ejercicio." << std::endl;
		MPI_Finalize();
		return 0;
	}

	initial_time = MPI_Wtime();
	if (rank == 0) {
		const int ping = 0;
		MPI_Send(&ping, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		std::cout << "Ping enviado por " << rank << std::endl;
		int pong;
		MPI_Recv(&pong, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::cout << "Pong recibido por " << rank << std::endl;
	} else if (rank == 1) {
		int ping;
		MPI_Recv(&ping, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::cout << "Ping recibido por " << rank << std::endl;
		const int pong = 1;
		MPI_Send(&pong, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		std::cout << "Pong enviado por " << rank << std::endl;
	}
	MPI_Barrier(MPI_COMM_WORLD);
	final_time = MPI_Wtime();
	if (rank == 0) {
		std::cout << "Tiempo total: " << final_time - initial_time << std::endl;
	}

	MPI_Finalize();
	return 0;
}