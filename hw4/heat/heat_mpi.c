/* diffusion1d_seq.c: sequential version of 1d diffusion.
 * The length of the rod is 1. The endpoints are frozen at 0 degrees.
 *
 * Author: Stephen F. Siegel <siegel@udel.edu>, September 2018
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <gd.h>
#include <mpi.h>
#include <unistd.h>
#define MAXCOLORS 256
#define INITIAL_TEMP 100.0
#define PWIDTH 2       // width of 1 conceptual pixel in real pixels
#define PHEIGHT 100    // height of 1 conceptual pixel in real pixels
#define TYPE MPI_DOUBLE


/* Global variables */
double k;                /* diffusivity constant, D*dt/(dx*dx) */
int nx;                  /* number of discrete points including endpoints */
int nsteps;              /* number of time steps */
int wstep;               /* write frame every wstep time steps */
double dx;               /* distance between two grid points: 1/(nx-1) */
double *u;               /* temperature function > now local to each proc */
double *u_new;           /* second copy of temp. > now local to each proc */
double *all;             /* array to hold all temp points*/
FILE *file;              /* file containing animated GIF */
gdImagePtr im, previm;   /* pointers to consecutive GIF images */
int *colors;             /* colors we will use */
int framecount = 0;      /* number of animation frames written */
int allIndex = 0;
double t0;
double t1;

int nprocs; // number of processes
int myrank; // my rank in MPI_COMM_WORLD
int first; // global index of first cell I own
int n_local; // how many cells do I "own"?
int left, right; // my left and right neighbors (ranks)

// n=number of elements, p=number of procs, r=rank
#define FIRST(r,N) ((N)*(r)/nprocs)
#define NUM_OWNED(r,N) (FIRST(r+1,N) - FIRST(r,N))
#define OWNER(j,N) ((nprocs*((j)+1)-1)/(N))
#define LOCAL_INDEX(j,N) ((j)-FIRST(OWNER(j,N),N))

/* init: initializes global variables. */
void init(int argc, char *argv[]) {
  if (argc != 6) {
    fprintf(stderr, "Usage: heat m k nx nsteps wstep filename\n");
    fprintf(stderr, "\
         k = diffusivity constant, D*dt/(dx*dx) (double)\n\
        nx = number of points in x direction, including endpoints (int)\n\
    nsteps = number of time steps (int)\n\
     wstep = write frame every this many steps (int)\n\
  filename = name of output GIF file (string)\n");
    fflush(stderr);
  }
  k = atof(argv[1]);
  nx = atoi(argv[2]);
  nsteps = atoi(argv[3]);
  wstep = atoi(argv[4]);
  file = fopen(argv[5], "wb");
  assert(file);
  //printf("Running heat with k=%.3lf, nx=%d, nsteps=%d, wstep=%d, filename=%s\n", k, nx, nsteps, wstep, argv[5]);
  fflush(stdout);
  assert(k>0 && k<.5);
  assert(nx>=2);
  assert(nsteps>=1);
  assert(wstep>=0);
  dx = 1.0/(nx-1);
  colors = (int*)malloc(MAXCOLORS*sizeof(int));
  assert(colors);
  all = (double*)malloc(nx*sizeof(double));
}

/* write_plain: write current data to plain text file and stdout */
void write_plain(int time) {
  FILE *plain;
  char filename[100], command[100];
  sprintf(filename, "./mpiout/out_%d", time);
  plain = fopen(filename, "w");
  assert(plain);
  for (int i = 0; i < nx; i++) fprintf(plain, "%8.2lf", all[i]);
  fprintf(plain, "\n");
  fclose(plain);
  sprintf(command, "cat %s", filename);
  system(command);
}

/* note from class: MPI_Gatherv(p+1, n_local, TYPE, all, counts,displs, TYPE, 0, MPI_COMM_WORLD); */

/*collects all pieces of u array from processes into all array so proc0 can write frames with later*/
void getAll(){
  if(myrank == 0) {
    MPI_Status status;
    int count;
    allIndex = n_local+1;   // +1 so start endpoint in all[] not overridden
    memcpy(all+1, u+1, (n_local)*(sizeof(double))); //all+1 is element after endpoint, u+1 is after ghostcell
    for (int proc=1; proc<nprocs; proc++) {
      //printf("index:%d count:%d n_local:%d",index, count,n_local);

      MPI_Recv(all+allIndex, n_local+1, TYPE, proc, 0, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, TYPE, &count);
      //printf("\nindex:%d count:%d n_local:%d ",allIndex, count,n_local);
      allIndex+=count;
    }
  }else{
      MPI_Send(u+1, n_local, TYPE, 0, 0, MPI_COMM_WORLD);
  }
}

/*write_frame: add a frame to animation
  only changed u[] array to all[] array
*/
void write_frame(int time) {
    im = gdImageCreate(nx*PWIDTH,PHEIGHT);
    if (time == 0) {
      for (int j=0; j<MAXCOLORS; j++)
        colors[j] = gdImageColorAllocate(im, j, 0, MAXCOLORS-j-1); 
      /* (im, j,j,j); gives gray-scale image */
      gdImageGifAnimBegin(im, file, 1, -1);
    } else {
      gdImagePaletteCopy(im, previm);
    }
    for (int i=0; i<nx; i++) {
      int color = (int)(all[i]*MAXCOLORS/INITIAL_TEMP);
      //printf("color:%d, all[i]:%f \n",color, all[i]);
      assert(color >= 0);
      if (color >= MAXCOLORS) color = MAXCOLORS-1;
      gdImageFilledRectangle(im, i*PWIDTH, 0, (i+1)*PWIDTH-1, PHEIGHT-1,colors[color]);
    }
    if (time == 0) {
      gdImageGifAnimAdd(im, file, 0, 0, 0, 0, gdDisposalNone, NULL);
    } else {
      // Following is necessary due to bug in gd.
      // There must be at least one pixel difference between
      // two consecutive frames.  So I keep flipping one pixel.
      // gdImageSetPixel (gdImagePtr im, int x, int y, int color);
      gdImageSetPixel(im, 0, 0, framecount%2);
      gdImageGifAnimAdd(im, file, 0, 0, 0, 5, gdDisposalNone, previm /*NULL*/);
      gdImageDestroy(previm);
    }
    previm=im;
    im=NULL;
  #ifdef DEBUG
    write_plain(time);
  #endif
    framecount++;
}

/* updates u for next time step. */
void update() {
  for (int i = 1; i <= n_local; i++){
      u_new[i] =  u[i] + k*(u[i+1] + u[i-1] -2*u[i]);
  }
  double *tmp = u_new; u_new = u; u = tmp; //swap so u is u_new
}  

void exchange_ghosts() {
  MPI_Sendrecv(u+1, 1, TYPE, left, 0,
	       u+(n_local+1), 1, TYPE, right, 0,
	MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  MPI_Sendrecv(u+n_local, 1, TYPE, right, 0,
	       u, 1, TYPE, left, 0,
	       MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

//for testing, prints everything in all buffer
void printAllBuffer(){
    if( myrank==0){
        for(int i = 0; i < nx; i++){
          printf("%f ", all[i]);
        }
        printf("\n");
  }
}

/* main: executes simulation.  Command line arguments:
 *         k = D*dt/(dx*dx) (double)
 *        nx = number of points in x direction, including endpoints (int)
 *    nsteps = number of time steps (int)
 *     wstep = write frame every this many steps (int)
 *  filename = name of output GIF file (string)
 */
int main(int argc, char *argv[]) {
  init(argc, argv);
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  first = FIRST(myrank,nx-2);  //note n-2 because not distributing endpoints
  n_local = NUM_OWNED(myrank,nx-2);

  MPI_Barrier(MPI_COMM_WORLD);
  if(myrank == 0){
      t0 = MPI_Wtime();
  }

  u = (double*)malloc((n_local+2)*sizeof(double));
  u_new = (double*)malloc((n_local+2)*sizeof(double));

  //endpoints are not distributed to procs & are not updated
  all[0] = 0.0;
  all[nx-1] = 0.0;

  for (int i = 0; i < n_local+2; i++){  //init all parts of rod to init temp 100
    u[i] = INITIAL_TEMP;
  } 

  if(n_local == 0 || first == 0){ //if first, set left ghost to temp 0
    left = MPI_PROC_NULL;
    u[0]=all[0];
  }else{
    left = OWNER(first-1,nx-2);
  }

  if(n_local == 0 || first+n_local >= nx-2){ //if last,set right ghost to 0
    right = MPI_PROC_NULL;
    u[n_local+1]=all[nx-1]; //n_local+1 because 1st ghost
  }else{
    right = OWNER(first+n_local,nx-2);
  }

  getAll();   //for writing frame 0
  if(myrank == 0){
    write_frame(0);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  exchange_ghosts();
  update();

  for (int time = 1; time <= nsteps; time++) {
    if (wstep != 0 && time%wstep==0){ //need != 0 check because modding divides, config3 gives divides by 0 error if no check
      getAll();
      if(myrank == 0){
        //printAllBuffer();
        write_frame(time);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    exchange_ghosts();
    update();
  }
  
  MPI_Barrier(MPI_COMM_WORLD);
  if(myrank == 0){
      t1 = MPI_Wtime();
      printf("Time (s): %3.2f\n", (t1-t0));
  }
  
  MPI_Finalize(); //processes keep working after this

  if(myrank == 0){
    gdImageDestroy(previm); //seg_faults if destroyed more than once!!
    gdImageGifAnimEnd(file);
    fclose(file);

    free(colors);
    free(all);
    free(u);
    free(u_new);
  }
  
}
