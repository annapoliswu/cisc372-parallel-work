/* Implementation of the evens library. */
#include <stdio.h>
#include "evens_lib.h"
#include <stdlib.h>

/*Note: calls malloc within program to return a new array, must store the returned array when calling this function, then free outside.*/
int * find_evens(int * p, int n, int * num_evens) {
  int evenCount = 0;
  int *a;
  for(int i = 0; i<n; i++){
    if(p[i] % 2 == 0){
      evenCount++;
    }
  }
  //num_evens must already be malloc-ed, can't malloc in function otherwise value set doesn't hold
  *num_evens = evenCount; 
  if(evenCount > 0){
    a = (int*) malloc(evenCount * sizeof(int));
    int c = 0;
    for(int i = 0; i<n; i++){
      if(p[i] % 2 == 0){
	a[c] = p[i];
	c++;
      }
    }
    return a;
  }else{
    return NULL;
  }
}

void print_array(int * p, int n) {
  printf("The even numbers are: ");
  for (int i = 0; i < n; i++)
    printf("%d ", p[i]);
  printf("\n");
  fflush(stdout);
}
