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
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  assert(argc==2);

  int nprocs, rank;
  double stop = (double)atol(argv[1]);

  assert(stop >= 1.0);
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int local_result = 0;
  int result = 0;
  double start_time = MPI_Wtime();

  for (double x = rank*1.0; x < stop; x += nprocs*1.0) {
    double tmp = sin(x);
    double tmp2 = tmp*tmp;
    int z = (int)(tmp2*10000.0);
    local_result = (local_result + z) % 10000;
  }
  MPI_Reduce(&local_result, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  result = result % 10000;
  
  double end_time = MPI_Wtime();

  if (rank == 0)
    printf("The PIN is %d (nprocs = %d, time = %f sec.)\n", result, nprocs, end_time-start_time);
  fflush(stdout);
  MPI_Finalize();
  return 0;
}
