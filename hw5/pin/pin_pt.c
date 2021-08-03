// The PIN is 6586 (Pthreads: nthreads = 40, time = .164 sec.)
// The PIN is 6586 (MPI: nprocs = 40, time = .138 sec.)
// ^ for input 100,000,000 : use make pt_test and mpi_test to run this number 


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
#include <pthread.h>
#include <sys/time.h>

int NUM_THREADS; //change in main doesn't get to sumPart(), pass as arg
int X;
int result = 0;
pthread_mutex_t tidsetup, writelock;

void *sumPart(void *);  

int main(int argc, char *argv[]) {
  assert(argc==3);

  X = (int)atol(argv[1]);
  assert(X >= 1.0);
  NUM_THREADS = (int)atol(argv[2]);
  assert(NUM_THREADS >= 1);
  pthread_mutex_init(&tidsetup,NULL);
  pthread_mutex_init(&writelock,NULL);
  
  
  struct timeval starttime, endtime;
  gettimeofday(&starttime, NULL);

  //holds NUM_THREADS, X, tid respectively
  int f_args[3];
  f_args[0] = NUM_THREADS;
  f_args[1] = X;
    
  pthread_t *threads = (pthread_t *)malloc(NUM_THREADS * sizeof(pthread_t));
  for (int tid = 0; tid < NUM_THREADS; tid++){
    pthread_mutex_lock(&tidsetup); //have to lock, otherwise can rewrite before thread reads
    f_args[2] = tid;    
    pthread_create(&threads[tid], NULL, sumPart, (void *) f_args);
  }
    
  for (int tid = 0; tid < NUM_THREADS; tid++){
    pthread_join(threads[tid], NULL);
  }
  
  gettimeofday(&endtime, NULL);
  double elapsed = (endtime.tv_sec - starttime.tv_sec) + (double)(endtime.tv_usec - starttime.tv_usec)/1000000.0;
  
  printf("The PIN is %d (nthreads = %d, time = %f sec.)\n", result, NUM_THREADS, elapsed);
  
  pthread_mutex_destroy(&tidsetup);
  pthread_mutex_destroy(&writelock);
  free(threads);
  fflush(stdout);
  return 0;

}

void * sumPart(void * f_args){
   
  int * args = (int *) f_args;
  int nthreads = args[0];
  int x = args[1];
  int id = args[2];

  pthread_mutex_unlock(&tidsetup); // safely ensues tid write and read are in one op
  
  //printf("id: %d, nthreads: %d, x: %d\n", id, nthreads,x);

  int localsum = 0;
  for(int i = id; i < x; i += nthreads){
    
    double tmp = sin(i);
    double tmp2 = tmp*tmp;
    int z = (int)(tmp2*10000.0);
    localsum += z;
    localsum = localsum % 10000;
  }

  //use localsum to reduce num of writes to global var; faster
  pthread_mutex_lock(&writelock);
  result = (result + localsum) % 10000;
  pthread_mutex_unlock(&writelock);
  
}
