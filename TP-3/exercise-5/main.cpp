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

int main(int argc, char *argv[]) {
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
		std::cerr << "Error al inicializar MPI" << std::endl;
		return 1;
	}

	MPI_Finalize();
	return 0;
}