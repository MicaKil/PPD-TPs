/* Programación Paralela y Distribuida, Lic. en Ciencias de la Computación, Facultad de Ingeniería, Universidad Nacional de Cuyo
 * Del Longo, Micaela
 * 8 de mayo de 2024
 *
 * Escribir un programa en MPI que implemente el clásico “Hola Mundo!” y además de saludar indique en qué nodo se
 * está ejecutando. */

#include <mpi.h>

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

	printf("Hello world from processor %s, rank %d out of %d processors\n", processor_name, world_rank, world_size);

	MPI_Finalize();
	return 0;
}