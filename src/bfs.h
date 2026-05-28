#ifndef BFS_H
#define BFS_H

#include "hashmap.h"
#include "street.h"
#include "queue.h"


void print_route_instructions(Path* path, Street* fromStreet, Street* toStreet);

/* 
 * Función principal del buscador GPS
 * Recibe:
 * - table: La HashTable con todas las calles indexadas (del Lab 5).
 * - fromStreet: Puntero a la calle donde empieza nuestro viaje (Origen).
 * - toStreet: Puntero a la calle donde queremos llegar (Destino).
 * * Esta función se encarga de buscar la ruta óptima en anchura, calcular
 * los giros matemáticos e imprimir las indicaciones finales por pantalla.
 */
void find_route_bfs(HashTable* table, Street* fromStreet, Street* toStreet);

#endif