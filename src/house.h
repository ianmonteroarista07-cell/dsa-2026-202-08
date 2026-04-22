#ifndef HOUSE_H
#define HOUSE_H

/*
   Struct House: Define un nodo de nuestra lista enlazada.
   Cada nodo representa una casa con su direccion y coordenadas.
*/
typedef struct House{
    char street_name[100];  // Nombre de la calle
    int house_number;       // Numero de la casa
    double lat;             // Latitud
    double lon;             // Longitud
    struct House* next;     // Puntero al siguiente nodo
} House;
// Pide y valida el nombre del mapa
void get_map_name(char* map_name);

// Cuenta las lineas de un archivo (es temporal esto)
int count_lines(char* map_name, char* file_name);

// Carga las casas del archivo houses.txt en una lista enlazada y devuelve head
House* load_houses(char* map_name); //Tambien es el contador de houses

// Busca una calle y numero específicos en la lista y muestra sus coordenadas
void find_house_coordinates(House* head, char* street, int number);

// Pide al usuario por teclado el nombre de una calle y un número
// street_final y num_final son punteros para que los datos vayan hacia el main
void get_adress_name(char* street_final, int* num_final);   

// Libera toda la memoria dinamica (malloc) usada por la lista al terminar
void free_houses(House* head);

// Traduce abreviaturas (C., Av., Ptge., Rbla.) a nombres completos
void normalize_street(char* destiny_street, char* original_street);

int levenshtein_distance(char *s1, char *s2);
#endif
