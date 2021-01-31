#include <stdio.h>

#include "mpi.h"

#include <time.h>

#include <stdlib.h>

int main(int argc, char ** argv) {

  int me; //process rank
  int root = 0; //0 is the root process
  int p; //# of processes
  int nrow; //# of rows
  int ncol; //# of columns
  int ndim = 2; //# of dimensions
  int dims[2];
  int periods[2];
  int reorder = 0; //process has no data
  int id2D; //id of process in the 2D matrix
  int coords2D[2]; //coordinates of process in 2D matrix
  int colID; //id of process in the 1D matrix
  int coords1D[1]; //coordinates of process in 1D matrix
  int belongs[2];
  int n; //# of numbers
  int loc_num; //# of numbers in each process
  int i;
  int fin[p];

  MPI_Comm comm2D, commcol;
  MPI_Init( & argc, & argv);
  MPI_Comm_rank(MPI_COMM_WORLD, & me);
  MPI_Comm_size(MPI_COMM_WORLD, & p);

  if (me == root) {

    printf("Type how many rows you want: ");
    scanf("%d", & nrow);

    printf("Type how many columns you want: ");
    scanf("%d", & ncol);

    printf("How many integers you want? ");
    scanf("%d", & n);
  }
  MPI_Bcast( & nrow, 1, MPI_INT, root, MPI_COMM_WORLD);
  MPI_Bcast( & ncol, 1, MPI_INT, root, MPI_COMM_WORLD);
  MPI_Bcast( & n, 1, MPI_INT, root, MPI_COMM_WORLD);
  int X[n]; //the numbers

  if (me == root) {
    int ans = 0;
    printf("How do you want to insert your data?\n");
    printf("1.Read from file.\n2.Type in console\n");
    do {
      scanf("%d", & ans);
      if (ans == 1) {
        FILE * f_read;
        char f_name[100];

        printf("Enter the name of your file: ");
        scanf("%s", f_name);

        if ((f_read = fopen(f_name, "r")) == NULL) {
          printf("Can't open file.\n");
          exit(1);
        }
        for (i = 0; i < n; i++) {
          fscanf(f_read, "%d", & X[i]);
        }
        fclose(f_read);

      } else if (ans == 2) {
        printf("Go\n");
        for (i = 0; i < n; i++) {
          scanf("%d", & X[i]);
        }
      }
    } while (!((ans == 1) || (ans == 2)));

  }

  MPI_Barrier(MPI_COMM_WORLD);

  dims[0] = nrow; //# of rows
  dims[1] = ncol; //# of columns
  periods[0] = 0; //NO cyclic rows
  periods[1] = 0; //NO cyclic columns

  MPI_Cart_create(MPI_COMM_WORLD, ndim, dims, periods, reorder, & comm2D); //create 2d topology
  MPI_Comm_rank(comm2D, & id2D);
  MPI_Cart_coords(comm2D, id2D, ndim, coords2D);

  MPI_Barrier(MPI_COMM_WORLD);

  loc_num = n / p; //# of data for each process
  int col_loc_matrix[loc_num];
  MPI_Scatter(X, loc_num, MPI_INT, col_loc_matrix, loc_num, MPI_INT, root, MPI_COMM_WORLD); //scatter the data

  MPI_Barrier(MPI_COMM_WORLD);

  belongs[0] = 1; //this dimension belongs to subgrid
  belongs[1] = 0; //this dimension does NOT belong to subgrid

  MPI_Cart_sub(comm2D, belongs, & commcol); //create column subgrids
  MPI_Comm_rank(commcol, & colID);
  MPI_Cart_coords(commcol, colID, 1, coords1D);

  MPI_Barrier(MPI_COMM_WORLD);

  int loc_sum = 0; //local sum for each process
  int col_fin = 0; //local sum for each column
  for (i = 0; i < loc_num; i++) {
    loc_sum += col_loc_matrix[i];
  }

  for (i = 0; i < nrow; i++) {
    MPI_Reduce( & loc_sum, & col_fin, 1, MPI_INT, MPI_SUM, i, commcol); //add the sum of local(process) integers in the first of their row
  }

  MPI_Gather( & col_fin, 1, MPI_INT, fin, 1, MPI_INT, root, MPI_COMM_WORLD); //gather all the sums

  MPI_Barrier(MPI_COMM_WORLD);

  int res = 0; //calculate final result
  for (i = 0; i < ncol; i++) { //add sums from the first process of each column
    res += fin[i];
  }
  if (me == root)
    printf("Result: %d\n", res); //print ANSWER

  MPI_Finalize();
  return 0;
}