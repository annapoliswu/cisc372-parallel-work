#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
//#include <sys/time.h>

//#include <opencv2/opencv.hpp>

__global__ void mykernel(int *arr, int *stop){
	int id = blockIdx.x * blockDim.x + threadIdx.x;  //unique global id of thread
	int numThreads = gridDim.x * blockDim.x;  //total num threads in grid in x direction
	int localsum = 0;
	
	for (int i = id; i < *stop; i+= numThreads){
		double tmp = sin(i*1.0);
		double tmp2 = tmp*tmp;
		int z = (int)(tmp2*10000.0);
		localsum = (localsum + z) % 10000;
	}
	printf(" %d ", localsum);
	
	arr[id] = localsum;
	/*
	if(id < *stop){
		double tmp = sin(id*1.0);
		double tmp2 = tmp*tmp;
		int z = (int)(tmp2*10000.0);
		arr[id] =  z % 10000;
	}
	*/
}


int main(int argc, char *argv[]){
	//assert(argc==2);
	int stop = (int)atol(argv[1]);
	assert(stop >= 1.0);

	printf("Hello World!\n");
	int blocks = 4;
	int threads = 5;
	int result = 0;

	int *arr;
	int arrsize; 
	if(blocks*threads < stop){
		arrsize = blocks*threads;
	}else{
		arrsize = stop;
	}
	arr = (int *)malloc(sizeof(int)*arrsize); //memory in cpu 
	
	int *devarr;
	int *devstop;
	cudaMalloc((int**) &devarr , sizeof(int)*arrsize); //mem in gpu
	cudaMalloc((int**) &devstop , sizeof(int)); //mem in gpu

	cudaMemcpy(devarr, arr, sizeof(int)*arrsize, cudaMemcpyHostToDevice); //transfer
	cudaMemcpy(devstop, &stop, sizeof(int), cudaMemcpyHostToDevice); //transfer
	mykernel<<<blocks,threads>>>(devarr, devstop);	//1,1 block, threads- launch config
	cudaMemcpy(arr, devarr, sizeof(int)*arrsize, cudaMemcpyDeviceToHost);
	
	printf("arrsize: %d\n", arrsize);
	
	for(int i = 0; i<arrsize; i++){
		//printf(" %d ", arr[i]);
		result = (result + arr[i]) % 10000;
	}
	
	printf("PIN is: %d\n",result);
	//scanf("%d%d", a, b);


	free(arr);
	cudaFree(devarr);
	cudaFree(devstop);

	return 0;
}