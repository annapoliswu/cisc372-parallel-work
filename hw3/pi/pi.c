#include<stdio.h>
#define INTERVALS 5000000000L
int main(int argc, char *argv[]) {
  long double area = 0.0;
  long double xi;
  long i;

  for (i=0; i<INTERVALS; i++) {
    xi=(1.0L/INTERVALS)*(i+0.5L); //midpoint of rect part
    area += 4.0L/(INTERVALS*(1.0L+xi*xi)); //height
  }
  printf("Pi is %20.17Lf\n", area);
}
