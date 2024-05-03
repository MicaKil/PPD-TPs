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
 * Para este ejercicio se requieren exactamente dos procesos.
 *
 * Para ejecutar el programa dentro de la carpeta <build> se debe ejecutar el siguiente comando:
 * mpirun -n 2 -oversubscribe TP3exercise4
 *
 * Ejercicio 4:
 *
 * Escribir un programa en MPI que implemente un “ping-pong”, esto es, un programa en el que un proceso envía un mensaje
 * a otro y este último lo devuelve inmediatamente al primero. Utilizar la función MPI_Wtime para calcular cuánto
 * tiempo se invierte en esta operación. */

#include <mpi.h>
#include <iostream>

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
			std::cerr << "Error: Se necesitan exactamente 2 procesos para este ejercicio." << std::endl;
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