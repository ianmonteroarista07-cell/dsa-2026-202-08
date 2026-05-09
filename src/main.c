#include "sample_lib.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "house.h"
#include "place.h"
#include "street.h"

/*
void createaleak() {
  char *foo = malloc(20 * sizeof(char));
  printf("Allocated leaking string: %s", foo);
}
*/
int main() {

  char map_name[20];
  int origin_position;

  // Para conseguir el nombre del mapa repetidas veces hasta que se introduzca uno correcto
  get_map_name(map_name);
  
  // Carga de datos, leemos el archivo houses.txt y places.txt creamos la lista enlazada
  // house_list guardará el puntero al primer nodo de nuestra lista. 
  // Tambien dará el nombre de houses cargadas
  House* house_list = load_houses(map_name);

  // place_list guardará el puntero al primer nodo de nuestra lista. 
  // Tambien dará el nombre de places cargados
  Place* place_list = load_places(map_name);

  // street_list guardará el puntero al primer nodo de nuestra lista.
  // Tambien dará el nombre de streets cargadas
  Street* street_list = load_streets(map_name);

  // Contamos las lineas de streets de manera temporal
  //printf("%d streets loaded\n", count_lines(map_name, "streets.txt"));

  // MENU DEL USUARIO 
  printf("\n\t--- PUNTO DE ORIGEN ---\n");
  printf("Donde estas? Address (1), Place (2) or Coordinate (3)?: ");

  // Leemos la opcion, si no es un numero, el switch irá al default.
  if(scanf("%d", &origin_position) != 1){
    printf("[ERROR] Entrada no válida.\n");
    free_houses(house_list); // Hay que liberar tambien
    free_places(place_list); 
    free_streets(street_list);
    return 1;
  }

  switch(origin_position){
    // El usuario quiere buscar por dirección (Calle + Numero)
    case 1:{
      char street_name[100];
      int num_street;
      // Pedimos al usuario el nombre de la calle y el numero
      get_adress_name(street_name, &num_street);
      // Si el numero es válido (distinto de -1)
      if(num_street != -1){
        // Buscamos las coordenadas (Lat, Lon) de esa casa especifica
        find_house_coordinates(house_list, street_name, num_street);
      }else{
        printf("[ERROR] Numero invalido.\n");
      }
      break;
    }
    //El usuario busca por nombre de lugar
    case 2: {
      char place_name[100];
      printf("Introduzca nombre del lugar (e.g. 'Sagrada Familia'): ");

      // Leemos el nombre del lugar permitiendo espacios
      if(scanf(" %99[^\n]", place_name) == 1){

        // Buscamos el lugar en la lista de 'Places'
        // Llamamos a la función que busca e imprime las coordenadas (ahora tambien guarda el nodo la funcion)
        Place* p = find_place_coordinates(place_list, place_name);

        // Si encontramos el lugar, extraemos su posición
        if(p != NULL){
          Position pos;
          pos.lat = p->lat;
          pos.lon = p->lon;

          // Buscamos cuál es el tramo de calle mas cercano a ese lugar
          Street* closest = get_closest_street(street_list, pos);

          // Si encontramos una calle cercana, mostramos sus conexiones
          if(closest != NULL){
            print_connected_streets(street_list, closest);
          } else{
              printf("\t[ERROR] No se han podido cargar calles o no hay ninguna cercana.\n");
          }

        }
      }
      break;
    }
    // Coordenadas manuales (Latitud / Longitud)
    case 3:
      printf("¡Aun no implementado!\n");
      break;

    // Si el usuario introduce un número que no es 1, 2 o 3
    default:
      printf("Opcion Invalida.\n");
      break;
  }
  // Recorremos toda la lista enlazada para liberar la memoria de cada nodo con free()
  free_houses(house_list);
  free_places(place_list);  
  free_streets(street_list);
  return 0;
}