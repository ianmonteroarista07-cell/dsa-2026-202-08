#ifndef STREETS_H
#define STREETS_H
#include <math.h>

// Estructura para guardar una coordenada
typedef struct{
    double lat;
    double lon;
} Position;

// Estructura de la lista enlazada para los tramos de calle
typedef struct Street{
    long long id1;  // ID del nodo donde empieza el tramo
    Position p1;    // Coordenadas del inicio
    long long id2;  // ID del nodo donde termina el tramo
    Position p2;    // Coordenadas del final
    double length;  // Longitud del tramo en metros
    char name[200]; // Nombre de la calle
    struct Street* next;    // Puntero al siguiente nodo
} Street; // Lo llamaria Streetsegment porque son tramos de calle y no la calle completa pero es muy largo

// Calcula el punto situado justo en medio de dos posiciones
Position midpoint(Position a, Position b);

// Abre el archivo del mapa y carga todos los tramos en la lista enlazada
Street* load_streets(char* map_name);

// Libera la memoria ocupada por la lista de calles al cerrar el programa
void free_streets(Street* head);

// Busca en la lista cual es el tramo mas cercano a la posicion del usuario
Street* get_closest_street(Street *head, Position userPos);

// Imprime por pantalla las calles a las que se puede girar desde la actual
void print_connected_streets(Street* head, Street* start_segment);

// Calcula la distancia real entre dos puntos teniendo en cuenta la curvatura de la Tierra (HAVERSINE)
double haversine(Position posA, Position posB);

#endif