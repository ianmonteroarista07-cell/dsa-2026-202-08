#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "house.h"
#include "place.h"
#include "street.h"
#include "hashmap.h"
#include "bfs.h"

int main(){
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

  // Declaramos la estructura de la tabla
  HashTable* street_table = malloc(sizeof(HashTable));
  if(street_table == NULL){
    printf("[ERROR] No hay memoria para la HashTable.\n");
    free_houses(house_list);
    free_places(place_list);
    free_streets(street_list);
    return 1;
  }

  // Configuramos el tamaño ideal de cajones buscando el nombre del mapa
  init_table(street_table, map_name);

  // Recorremos la lista de streets y las clasificamos todas dentro de los cajones
  full_table(street_table, street_list);
  // Contamos las lineas de streets de manera temporal
  //printf("%d streets loaded\n", count_lines(map_name, "streets.txt"));

  // MENU DEL USUARIO 
  printf("\n\t--- PUNTO DE ORIGEN ---\n");
  printf("¿Donde estas? Address (1), Place (2) or Coordinate (3)?: ");

  // Leemos la opcion, si no es un numero, el switch irá al default.
  if(scanf("%d", &origin_position) != 1){
    printf("[ERROR] Entrada no válida.\n");
    free_houses(house_list); // Hay que liberar tambien
    free_places(place_list); 
    free_streets(street_list);
    free_table(street_table); // <-- [LAB 5] Liberamos también aquí si el usuario se equivoca
    free(street_table);
    return 1;
  }
  while(origin_position <= 0 || origin_position > 3){
    printf("[ERROR] Entrada no válida. Números válidos [1, 2, 3]: ");
    if(scanf("%d", &origin_position) != 1){
      printf("[ERROR] Entrada no válida.\n");
      free_houses(house_list); // Hay que liberar tambien
      free_places(place_list); 
      free_streets(street_list);
      free_table(street_table); // <-- [LAB 5] Liberamos también aquí si el usuario se equivoca
      free(street_table);
      return 1;
    }
  }  

  // VARIABLES PARA LOS 3 CASOS
  /*  
    Creem una única variable de posició per guardar l'origen final trobat. 
    D'aquesta manera, sigui quina sigui l'opció que triï l'usuari (1, 2 o 3),
    totes acabaran guardant la latitud i longitud aquí
  */
  Position pos_origen;
  int listo_parabuscar = 0;

  switch(origin_position){
    // El usuario quiere buscar por dirección (Calle + Numero)
    case 1: {
      char street_name[100];
      int num_street;
      // Pedimos al usuario el nombre de la calle y el numero
      get_adress_name(street_name, &num_street);
      // Si el numero es válido (distinto de -1)
      if(num_street != -1){
        // Buscamos las coordenadas (Lat, Lon) de esa casa especifica
        House* h = find_house_coordinates(house_list, street_name, num_street);
        if(h != NULL){
          pos_origen.lat = h->lat;
          pos_origen.lon = h->lon;
          listo_parabuscar = 1; // Activem que hem trobat les coordenades
        }
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
          pos_origen.lat = p->lat;
          pos_origen.lon = p->lon;
          listo_parabuscar = 1;
        }
      }
      break;
    }
    // Coordenadas manuales (Latitud / Longitud)
    case 3: {
      printf("Introduzca Latitud (e.g. 41.384532): ");
      if(scanf("%lf", &pos_origen.lat) != 1){
        printf("[ERROR] Latitud Inválida.\n");
        break;
      }
      printf("Introduzca Longitud (e.g. 2.170421): ");
      if(scanf("%lf", &pos_origen.lon) != 1){
        printf("[ERROR] Longitud Inválida.\n");
        break;
      }
      /* En aquest cas no hem de buscar a cap llista. Les dades que ha escrit 
      l'usuari ja són les coordenades directes, així que activem la bandera. */
      listo_parabuscar = 1; 
      break;
    }
    // Esto era por si el usuario no introducia ni 1, 2 ni 3, pero ya no hace falta, pero lo dejamos por si acaso.
    default:
      printf("Opcion Invalida.\n");
      break;
  }

  // ESTE ES EL BLOQUE FINAL COMÚN

  // si listo_parabuscar es 1, significa que cualquiera de los 3 casos de arriba ha guardado las coordenadas
  // con éxito en pos_origen, ara ejecutamos el calculo del mapa
  Street* closest = NULL;
  if(listo_parabuscar){
    
    // Buscamos cuál es el tramo de calle mas cercano a nuestro 'pos_origen'
    closest = get_closest_street(street_list, pos_origen);

    // Si encontramos una calle cercana, mostramos sus conexiones
    if(closest != NULL){
      // [VERSION LAB 4] - Búsqueda lineal lenta
      printf("\n--- [LAB 4] BUSCANDO CONEXIONES LINEALES (LENTO) ---\n");
      print_connected_streets(street_list, closest);

      // [VERSION LAB 5] - Búsqueda rápida
      printf("\n--- [LAB 5] BUSCANDO CONEXIONES CON HASHTABLE (RÁPIDO) ---\n");
      print_connected_streets_hash(street_table, closest);
    }else{
      printf("\t[ERROR] No se han podido cargar calles o no hay ninguna cercana.\n");
    }
  }

  // --- [LAB 6] PUNTO DE DESTINO --
  int destination_position = 0; // Almacena la opción que elija el usuario (1, 2 o 3)
  Position pos_destination;     // Estructura para guardar la Latitud y Longitud finales del destino
  int listo_paradestino = 0;    // Cambiará a 1 si encontramos con éxito las coordenadas de destino
  Street* closest_dest = NULL;  // Puntero para guardar el tramo de calle más cercano al destino

  // Solo pedimos destino si el origen se pudo calcular correctamente
  if(listo_parabuscar && closest != NULL){
    printf("\n\t--- PUNTO DE DESTINO ---\n");
    printf("¿Donde quieres ir? Address (1), Place (2) or Coordinate (3)?: ");

    // Unicamente aceptamos 1, 2, 3 como opciones.
    if(scanf("%d", &destination_position) != 1){
      printf("[ERROR] Entrada no válida para el destino.\n");
      free_houses(house_list); 
      free_places(place_list); 
      free_streets(street_list);
      free_table(street_table);
      free(street_table);
      return 1;
    }
    while(destination_position <= 0 || destination_position > 3){
      printf("[ERROR] Entrada no válida. Números válidos [1, 2, 3]: ");
      if(scanf("%d", &destination_position) != 1){
        printf("[ERROR] Entrada no válida.\n");
        free_houses(house_list); 
        free_places(place_list); 
        free_streets(street_list);
        free_table(street_table);
        free(street_table);
        return 1;
      }
    }

    // Evaluamos el caso según lo que haya escrito el usuario
    switch(destination_position){
      case 1: {
        char street_name[100];  // Guardar nombre calle
        int num_street;     // Guardar numero casa
        // Llamamos a la función para pedir la calle y el número por pantalla
        get_adress_name(street_name, &num_street);
        if(num_street != -1){
          // Buscamos las coordenadas (Lat, Lon) de esa casa en la lista de houses
          House* h = find_house_coordinates(house_list, street_name, num_street);
          if(h!= NULL){
            pos_destination.lat = h->lat;
            pos_destination.lon = h->lon;
            listo_paradestino = 1;
          }
        }else{
          printf("[ERROR] Numero invalido.\n");
        }
        break;
      }
      case 2: {
        char place_name[100]; // Guardar nombre del sitio
        printf("Enter place name (e.g. 'L'Illa Diagonal'): ");
        if(scanf(" %99[^\n]", place_name) == 1){
          // Buscamos el lugar en la lista de sitios (places)
          Place* p = find_place_coordinates(place_list, place_name);
          if(p != NULL){
            pos_destination.lat = p->lat;
            pos_destination.lon = p->lon;
            listo_paradestino = 1;
          }
        }
        break;
      }
      case 3: {
        printf("Introduzca Latitud de destino: ");
        // Si el usuario escribe algo que no es un número decimal (double), da error
        if(scanf("%lf", &pos_destination.lat) != 1){
          printf("[ERROR] Latitud Inválida.\n");
          break;
        }
        printf("Introduzca Longitud de destino: ");
        if(scanf("%lf", &pos_destination.lon) != 1){ 
          printf("[ERROR] Longitud Inválida.\n");
          break;
        }
        listo_paradestino = 1;
        break;
      }

      // Esto era por si el usuario no introducia ni 1, 2 ni 3, pero ya no hace falta, pero lo dejamos por si acaso.
      default:{
        printf("Opcion Invalida para el destino.\n");
        break;
      }
    }
  }

  // Si tanto el origen como el destino se han encontrado con éxito  (1)
  if(listo_parabuscar && listo_paradestino){
    // Buscamos cuál es la calle o tramo real más cercano a las coordenadas de destino
    closest_dest = get_closest_street(street_list, pos_destination);

    // Si encontramos una calle cerca del destino
    if(closest_dest != NULL){
      // Imprimimos los datos geométricos de la calle de llegada tal y como pide el ejemplo del enunciado
      printf("\n\tClosest street: %s\n", closest_dest->name);
      printf("\tBetween %lld (%lf, %lf) and %lld (%lf, %lf)\n", closest_dest->id1, closest_dest->p1.lat, closest_dest->p1.lon, 
        closest_dest->id2, closest_dest->p2.lat, closest_dest->p2.lon);
             
      // Imprimimos la cabecera inicial que pide la sección de la ruta
      printf("\n\n--- ROUTE ---\n");
  
      find_route_bfs(street_table, closest, closest_dest);
      
    }else{
      printf("\t[ERROR] No se ha encontrado ninguna calle cercana al destino.\n");
    }
  }
  // Recorremos toda la lista enlazada para liberar la memoria de cada nodo con free()
  free_houses(house_list);
  free_places(place_list);  
  free_streets(street_list);

  free_table(street_table); // Vaciamos las listas y el array de buckets por dentro
  free(street_table);       // Borramos la propia HashTable de la memoria
  return 0;
}