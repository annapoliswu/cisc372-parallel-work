/* Program for ragged question */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: create two global variables: 
//   1. lengths. Type: pointer-to-int
//   2. data. Type: pointer-to-pointer-to-int.
int * lengths;
int ** data;


/* Main first allocates memory for lengths and copy-converts the array of char arguments to ints.
 * Then it allocates memory for the pointers to pointers in data. It loops through rows 0 - n
 * allocating memory for the ints in each row, then sets the value of the int and increases the
 * count so the values will increase every index. Finally it goes through the data array
 * printing every index and freeing the rows once they're done with. Then it frees everything
 * else that was dynamically allocated.  
 */
int main(int argc, char **argv) {

  int rows = argc-1;
  
  lengths = (int*)malloc( rows * sizeof(int));
  for(int i=0; i<rows; i++){
    lengths[i] = atoi(argv[i+1]);  //+1 necessary because argv[0] is program name
  }
  
  int **data = (int **) malloc(rows * sizeof(int *));
  int num = 0;
  
  for(int n = 0; n < rows; n++){
    int cols = lengths[n];
    data[n] = (int*) malloc(cols *sizeof(int));
    for( int m = 0; m<cols; m++){
      data[n][m] = num;
      num++;
    }
  }

  for(int n =0; n < rows; n++){    
    for(int m = 0; m < lengths[n]; m++){
      printf("%d ", data[n][m]);
    }
    printf("\n");    
    free(data[n]);
  }
  free(data);
  free(lengths);
  return 0;
  
}
