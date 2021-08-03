#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h> 

/****************************************/

#define NPINGS 1000000

int nprocs;
int rank;
int first;
int n_local;

int main(int argc, char *argv[]) {
  double walltime = 0;
  int count = 0;
  char message[1];
  double t0;
  double t1;
  
  MPI_Status status;
  MPI_Init(NULL, NULL); 
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
     
  for(int proc = 1; proc < nprocs; proc++){

    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0){//for if first process encountered is not proc0, start time on proc0
      t0 = MPI_Wtime();
    }

    for(int i = 0; i < NPINGS; i++){
      if(rank == 0){
	MPI_Send(message, 0, MPI_CHAR, proc, 99, MPI_COMM_WORLD);
	MPI_Recv(message, 0, MPI_CHAR, proc, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	      
      }else if(rank == proc){
	MPI_Recv(message, 0, MPI_CHAR, 0, 99, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Send(message, 0, MPI_CHAR, 0, 99, MPI_COMM_WORLD);
	
      }
    } //end ping for 

    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0){ //proc 0 takes end time
      t1 = MPI_Wtime();
      walltime = t1-t0;
      walltime = walltime / (2*NPINGS);
      printf("Average time to transmit between 0 and %d: %11.10f\n", proc, walltime);
      fflush(stdout);
    }
  } //end proc for
  
  //printf("check\n");
  MPI_Finalize();
  
}
