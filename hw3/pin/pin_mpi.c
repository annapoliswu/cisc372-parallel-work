// The PIN is 913 (nprocs = 40, time = 17.57 sec.)


/* This program computes a rather funny function on integers.
 * Given any nonnegative integer i, let f(i) be the integer
 * specified by the first 4 digits after the decimal point in the decimal
 * expansion of sin^2(i).
 * Given any nonnegative integer n, let g(n) be the sum, over all i
 * in 0..(n-1), of f(i).  This program computes g(n)%10000.
 * The result in an integer in 0..9999.
 * The argument n is specified as the sole argument on the command line.
 * It must be representable as a long int.
 * The program prints the result at the end.
 * Some tests:
 * 1 -> 0
 * 2 -> 7080
 * 100 -> 0076
 *
 * 100000000 takes 4.2 seconds on my MacBook Air
 * 1000000000  takes 43 seconds on same machine
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h> 

int nprocs;
int rank;
int first;
int n_local;

int main(int argc, char *argv[]) {
  assert(argc==2);
  double stop = (double)atol(argv[1]);
  assert(stop >= 1.0);

  int localsum = 0;
  int result = 0;
  double walltime = 0;
  
  MPI_Status status;
  MPI_Init(&argc, &argv); //if want use command args
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); //gets rank of process
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs); // num of processes

  //NOTE: C will auto floor in #define function, but not in main?? Have to call floor from math
  first = floor(stop*(double)rank/(double)nprocs);
  n_local = floor(stop*(double)(rank+1)/(double)nprocs) - first;
  double t0 = MPI_Wtime();
  
  for(int i = 0; i < n_local; i++){
    int globi = first+i;
    double tmp = sin(globi);
    double tmp2 = tmp*tmp;
    int z = (int)(tmp2 * 10000.0);
    localsum += z;
    localsum = localsum %10000;
  }

  MPI_Reduce(&localsum, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  result = result % 10000;
  double t1 = MPI_Wtime();
  MPI_Finalize();

  walltime = t1-t0;
  if(rank == 0){
    printf("The PIN is %d (nprocs = %d, time = %.2f sec.)\n", result, nprocs, walltime);
  }
  fflush(stdout);
  
}




/*

//IGNORE, old solution, not fast; not contiguous ??

int main(int argc, char *argv[]) {
  int rank, size;
  int root = 0;
  double walltime = 0;
  MPI_Status status;
  MPI_Init(&argc, &argv); //if want use command args
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); //gets rank of process
  MPI_Comm_size(MPI_COMM_WORLD, &size); // num of processes
  assert(argc==2);


  double stop = (double)atol(argv[1]);

  assert(stop >= 1.0);

  int result = 0;
  int localsum = 0;
  
  MPI_Bcast(&stop, 1, MPI_DOUBLE, root, MPI_COMM_WORLD); 
  

  for(int i = rank; i < stop; i += size){ //starts from rank increases every interval, across all processes covers
    double tmp = sin(i);
    double tmp2 = tmp*tmp;
    int z = (int)(tmp2 * 10000.0);
    localsum += z;
    //printf("rank: %d, localsum: %d\n", rank, localsum);

  }
    localsum = localsum % 10000;  
  
  MPI_Reduce(&localsum, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  result = result % 10000;
  
  if(rank == 0){
    printf("The PIN is %d (nprocs = %d, time = %f)\n", result, size, walltime);
  }
  fflush(stdout);

  MPI_Finalize();

}
*/
