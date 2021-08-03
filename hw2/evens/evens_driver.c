/* Driver for evens libary */
#include "evens_lib.h"
#include <stdlib.h>
#include <stdio.h>

/* Main first allocates memory for the pointer-to-int number of even numbers, the array of args,
 * and sets up the other variables needed. It then converts the char argv array to a pointer-to-int
 * so that it can be used in the find_evens function. It calls the find_evens function - which changes
 * the value of evensLength - and stores the returned pointer-to-int in evensArr. Finally it prints
 * the evensArr by calling print_array and frees all previously allocated memory.  
 */
int main(int argc, char *argv[]) {

  int length = argc-1;
  int * evensLength = (int * ) malloc(length * sizeof(int)); 
  int * arr = (int *) malloc(length * sizeof(int));
  int * evensArr;
 
  for(int i = 0; i < length; i++){
    arr[i] = atoi(argv[i+1]);
  }
  
  evensArr = find_evens(arr, length, evensLength);
  print_array(evensArr, *evensLength);
  printf("\n");

  free(evensLength);
  free(arr);
  free(evensArr);
  return 0;
}
