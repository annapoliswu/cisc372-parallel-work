#include<stdio.h>
#include <mpi.h> 
#include <math.h>

#define INTERVALS 5000000000L
#define FIRST(r) ((INTERVALS)*(r)/nprocs)
#define NUM_OWNED(r) (FIRST(r+1) - FIRST(r))

int nprocs;
int rank;
long first;
long n_local;

int main(int argc, char *argv[]) {
  long double area = 0.0;
  long double localarea = 0.0;
  long double xi;
  long i;

  
  MPI_Status status;
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  first = FIRST(rank);
  n_local = NUM_OWNED(rank);

  for (i=0; i<n_local; i++) {
    long globi = first+i;
    xi=(1.0L/INTERVALS)*(globi+0.5L); //midpoint of rect part
    localarea += 4.0L/(INTERVALS*(1.0L+xi*xi)); //height
  }
  MPI_Reduce(&localarea, &area, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if(rank == 0){
    printf("Pi is %20.17Lf\n", area);
  }
  fflush(stdout);
  MPI_Finalize();

}


 
