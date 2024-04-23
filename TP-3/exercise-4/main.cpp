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

	int world_size, world_rank;

	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	if (world_size != 2) {
		if (world_rank == 0)
			std::cerr << "Se necesitan exactamente 2 procesos para este ejemplo." << std::endl;
		return 1;
	}

	initial_time = MPI_Wtime();
	if (world_rank == 0) {
		const int ping = 0;
		MPI_Send(&ping, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		std::cout << "Ping enviado por " << world_rank << std::endl;
		int pong;
		MPI_Recv(&pong, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::cout << "Pong recibido por " << world_rank << std::endl;
	} else if (world_rank == 1) {
		int ping;
		MPI_Recv(&ping, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		std::cout << "Ping recibido por " << world_rank << std::endl;
		const int pong = 1;
		MPI_Send(&pong, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		std::cout << "Pong enviado por " << world_rank << std::endl;
	}
	MPI_Barrier(MPI_COMM_WORLD);
	final_time = MPI_Wtime();
	if (world_rank == 0) {
		std::cout << "Tiempo total: " << final_time - initial_time << std::endl;
	}

	MPI_Finalize();
	return 0;
}