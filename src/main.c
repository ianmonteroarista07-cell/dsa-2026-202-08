#include "sample_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "house.h"
#include "place.h"


void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}

int main() {

  char map_name[20];
  int origin_position;

  // Para conseguir el nombre del mapa repetidas veces hasta que se introduzca uno correcto
  get_map_name(map_name);
  
  // Carga de datos, leemos el archivo houses.txt y creamos la lista enlazada
  // house_list guardará el puntero al primer vagón de nuestra lista. 
  // Tambien dará el nombre de casas cargadas
  House* house_list = load_houses(map_name);

  // Contamos las lineas de los places y de streets de manera temporal
  //printf("%d places loaded\n", count_lines(map_name, "places.txt"));
  Place* place_list = load_places(map_name);
  printf("%d streets loaded\n", count_lines(map_name, "streets.txt"));

  // MENU DEL USUARIO 
  printf("\n\t--- PUNTO DE ORIGEN ---\n");
  printf("Donde estas? Address (1), Place (2) or Coordinate (3)?: ");

  // Leemos la opcion, si no es un numero, el switch irá al default.
  if(scanf("%d", &origin_position) != 1){
    printf("[ERROR] Entrada no válida.\n");
    free_houses(house_list); // Hay que liberar tambien
    free_places(place_list); 
    return 1;
  }

  switch(origin_position){
    case 1:{
      char street_name[100];
      int num_street;
      get_adress_name(street_name, &num_street);
      if(num_street != -1){
        find_house_coordinates(house_list, street_name, num_street);
      }else{
        printf("[ERROR] Numero invalido.\n");
      }
      break;
    }
    case 2: {
    char place_name[100];
      printf("Introduzca nombre del lugar (e.g. 'Sagrada Familia'): ");

      scanf(" %99[^\n]", place_name); 
      
      // Llamamos a la función que busca e imprime las coordenadas
      find_place_coordinates(place_list, place_name);
      break;
    }
    case 3:
      printf("¡Aun no implementado!\n");
      break;
    
    default:
      printf("Opcion Invalida.\n");
      break;
  }
  // Recorremos toda la lista enlazada para liberar la memoria de cada nodo con free()
  free_houses(house_list);
  free_places(place_list);  
  return 0;
}