#ifndef HASHMAP_H
#define HASHMAP_H
#include "street.h"


typedef struct HashNode{
    Street* segment;        // Puntero al segmento de calle 
    struct HashNode* next;  // Puntero al siguiente nodo (por si hay un choque en el mismo cajón)
} HashNode;

typedef struct{
    HashNode** buckets;    // Array dinámico de punteros a HashNode (es decir, la lista de cajones).
    int size;              // Tamaño total de la tabla (varía según el mapa)
} HashTable;

// Configura el tamaño según el mapa, hace el malloc del array interno y lo pone a NULL
void init_table(HashTable* table, char* map_name);

// Calcula el índice del cajón haciendo: ID % table_size
int hash_function(long long id, int table_size);

// Inserta un tramo de calle en el cajón de una esquina 
void insert_table(HashTable* table, long long id, Street* segment);

// Recorre la lista de calles antigua y las clasifica dentro del hashtable
void full_table(HashTable* table, Street* street_list);

// Muestra a qué calles puedes ir desde la calle actual usando la HashTable
void print_connected_streets_hash(HashTable* table, Street* start_segment);

// Libera toda la memoria de los HashNode internos y del array de buckets
void free_table(HashTable* table);

#endif