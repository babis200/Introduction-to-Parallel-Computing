	#include <mpi.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <math.h>

	int main(int argc, char** argv) {

		int X[21] = {10, 28, 31, 79, 8, 30, 68, 75, 66, 29, 57, 38, 97, 95, 82, 15, 61, 18, 88, 64, 5};
		//int X[24] = {67, 59, 52, 4, 92, 16, 13, 51, 93, 94, 80, 77,
		//27, 83, 34, 47, 33, 12, 44, 26, 86, 10, 71, 52};
		//int X[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
		int my_rank;
		int p; //plhthos processess
		int loc_num; //poso antistoixei sth kathe process
		int root = 0, n, min = X[0], max = 0;
		int fin_above_m = 0; //synoliko plhthos akeraiwn > avg
		int fin_bellow_m = 0; //synoliko plhthos akeraiwn < avg
		int loc_above_m = 0; //topiko plhthos akeraiwn > avg
		int loc_bellow_m = 0; //topiko plhthos akeraiwn > avg
		float avg=0;

		MPI_Init(&argc, &argv);
		MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
		MPI_Comm_size(MPI_COMM_WORLD, &p);

		n = sizeof(X) / sizeof(int); //plhthos timwn

		if (my_rank == root) {

			int i;

			for (i=0;i<n;i++) { // calculate min max avg

				avg = avg + ((float)X[i] / n);

				if (X[i] > max)
				max = X[i];

				if (X[i] < min)
				min = X[i];
			}
		}

		MPI_Bcast(&min, 1, MPI_INT, root, MPI_COMM_WORLD);
		MPI_Bcast(&max, 1, MPI_INT, root, MPI_COMM_WORLD);
		MPI_Bcast(&avg, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);


		loc_num = n/p;
		int loc_matrix[loc_num];
		MPI_Scatter(X, loc_num, MPI_INT, loc_matrix, loc_num, MPI_INT, root, MPI_COMM_WORLD);


		int i;
		// ----------------------------------- A ----------------------------------------------
		for(i=0;i<loc_num;i++) {
			if(avg < loc_matrix[i])
			loc_above_m += 1;
			else if(avg > loc_matrix[i])
			loc_bellow_m += 1;
		}

		MPI_Reduce(&loc_above_m, &fin_above_m, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
		MPI_Reduce(&loc_bellow_m, &fin_bellow_m, 1, MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);

		MPI_Barrier(MPI_COMM_WORLD);
		if (my_rank == root) {
			printf("Πλήθος ακεράιων στο διάνυσμα Χ μεγαλύτεροι του m:\t%d\n", fin_above_m);
			printf("Πλήθος ακεράιων στο διάνυσμα Χ μικρότεροι του m:\t%d\n", fin_bellow_m);

		}
		MPI_Barrier(MPI_COMM_WORLD);
		// ----------------------------------- A ----------------------------------------------

		// ----------------------------------- B ----------------------------------------------

		float fin_var = 0; //olikh diaspora
		float loc_var = 0; //topikh diaspora
		for(i=0;i<loc_num;i++) {
			loc_var += pow((loc_matrix[i] - avg), 2);
		}

		MPI_Reduce(&loc_var, &fin_var, 1, MPI_FLOAT, MPI_SUM, root, MPI_COMM_WORLD);

		MPI_Barrier(MPI_COMM_WORLD);
		if (my_rank == root) {
			fin_var = fin_var / n;
			printf("Διασπορά στο διάνυσμα Χ :\t%.3f\n", fin_var);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		// ----------------------------------- B ----------------------------------------------

		// ----------------------------------- Γ ----------------------------------------------


		float d_loc_matrix[loc_num];
		float d_fin_matrix[n];

		for(i=0;i<loc_num;i++)
		//printf("loc_matrix[%d] = %d\n", i, loc_matrix[i]);
		//if((max - loc_matrix[i])!=0)
		d_loc_matrix[i] = ((float)(loc_matrix[i] - min) / (float)(max - min) )* 100;
		//else
		// d_loc_matrix[i] = 0;

		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Gather(d_loc_matrix, loc_num, MPI_FLOAT, d_fin_matrix, loc_num, MPI_FLOAT, root, MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		if (my_rank == root) {
			for(i=0;i<n;i++)
			printf("Δ[%d] = %.3f\n", i, d_fin_matrix[i]);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		// ----------------------------------- Γ ----------------------------------------------

		// ----------------------------------- Δ ----------------------------------------------

		int d_loc_max[2]; //d_loc_max[0] = topiko megisto
		//d_loc_max[1] = thesi topikou megistou
		int d_fin_max[2]; //d_fin_max[0] = olika megisto megisto
		//d_fin_max[1] = thesi topikou megistou

		d_loc_max[0] = d_loc_matrix[0];
		d_loc_max[1] = loc_num*(my_rank) + 0;
		for(i=1;i<loc_num;i++) {
			if(d_loc_matrix[i] > d_loc_max[0]) {
				d_loc_max[0] = d_loc_matrix[i];
				d_loc_max[1] = loc_num*(my_rank) + i ; //e.g.
			}
		}
		MPI_Reduce(&d_loc_max, &d_fin_max, 1, MPI_2INT, MPI_MAXLOC, root, MPI_COMM_WORLD);

		MPI_Barrier(MPI_COMM_WORLD);
		if (my_rank == root) {
			printf("Η μέγιστη τιμή στο διάνυσμα Δ είναι %d στη θέση %d και η τιμή του στο διάνυσμα Χ είναι %d .\n", d_fin_max[0], d_fin_max[1],
			X[d_fin_max[1]]);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		// ----------------------------------- Δ ----------------------------------------------
		// ----------------------------------- E ----------------------------------------------

		if(n == p) {
			int p_sums[n];

			MPI_Scan(&loc_matrix,&p_sums, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);


			MPI_Barrier(MPI_COMM_WORLD);

			printf("X[%d] = %d\tprefix sums = %d\n", my_rank, loc_matrix[0], p_sums[0]);

		}/

		MPI_Finalize();
		return 0;
	}