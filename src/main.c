#include "sample_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>



void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {
  
  char map_name[20];
  int origin_position;
  char street_name[100];
  int street_number;

  printf("Introduzca el nombre del mapa (e.g. 'xs_2' o 'xl_1'): ");
  scanf("%s", map_name);

  printf("Donde estas? Address (1), Place (2) or Coordinate (3)?: ");
  scanf("%d", &origin_position);

  switch(origin_position){
    case 1:
      printf("Introduzca nombre de la calle (e.g. 'Carrer de Roc Boronat'): ");
      scanf("%s", street_name);

      printf("Introduzca numero de la calle (e.g. '138'): ");
      scanf("%d", &street_number);
      break;
    case 2:
      printf("¡Aun no implementado!");
      break;
    case 3:
      printf("¡Aun no implementado!");
      break;
    
  }
  // how to import and call a function
  printf("el megafaaaccctorial of 4 is %d\n", fact(4));

  // uncomment and run "make v" to see how valgrind detects memory leaks
  // createaleak();

  return 0;
}