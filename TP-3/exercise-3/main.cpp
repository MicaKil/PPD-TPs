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
 * Para ejecutar el programa dentro de la carpeta <build> se debe ejecutar el siguiente comando:
 * mpirun -n <Núm. de procesos> -oversubscribe TP3exercise3
 *
 * Ejercicio 3:
 *
 * Escribir un programa en MPI que implemente el clásico “Hola Mundo!” y además de saludar indique en qué nodo se
 * está ejecutando. */

#include <mpi.h>
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[]) {
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		std::cerr << "Error al inicializar MPI" << std::endl;
		return 1;
	}

	int world_size, world_rank;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);

	printf("¡Hola Mundo! desde el nodo %s, rank %d de %d procesos.\n", processor_name, world_rank, world_size);

	MPI_Finalize();
	return 0;
}