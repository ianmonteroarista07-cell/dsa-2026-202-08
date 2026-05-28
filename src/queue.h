#ifndef QUEUE_H
#define QUEUE_H

#include "street.h"

/* 
 * Un "Path" representa un camino. Guarda una secuencia de calles 
 * en un array y un entero para saber cuántas calles tiene el camino.
 */
typedef struct{
    Street* streets[500]; // Guardamos hasta 500 calles consecutivas de la ruta
    int count;            // Cuántas calles hay guardadas en este momento
} Path;

/* 
 * Un nodo de la cola. El BFS necesita guardar caminos enteros en la cola,
 * así que cada nodo de la cola almacena una estructura Path completa.
 */
typedef struct QueueNode{
    Path camino;             // El camino guardado en este nodo
    struct QueueNode* next;  // Puntero al siguiente nodo de la cola
} QueueNode;

/* 
 * Estructura de la Cola para saber por dónde entran 
 * y por dónde salen los elementos.
 */
typedef struct{
    QueueNode* front; // Primer elemento (por aqui sacamos con dequeue)
    QueueNode* rear;  // Último elemento (por aqui metemos con enqueue)
} Queue;

// Funciones que usaremos en el BFS para manejar la cola
void init_queue(Queue* q);
int is_empty(Queue* q);
void enqueue_path (Queue* q, Path p);
Path dequeue_path(Queue* q);
void free_queue(Queue* q);

#endif