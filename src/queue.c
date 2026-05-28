#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Inicializa la cola poniendo los punteros a NULL (cola vacía)
void init_queue(Queue* q){
    q->front = NULL;
    q->rear = NULL;
}

// Devuelve 1 si la cola está vacía, o 0 si tiene elementos
int is_empty(Queue* q){
    if (q->front == NULL){
        return 1;
    }
    return 0;
}

// Mete un camino entero al final de la cola (por el Rear)
void enqueue_path(Queue* q, Path p){
    // Pedimos memoria para el nuevo nodo de la cola
    QueueNode* new_node = malloc(sizeof(QueueNode));
    if(new_node == NULL) return; // Si no hay memoria, salimos
    
    new_node->camino = p;    // Guardamos el camino dentro del nodo
    new_node->next = NULL;   // De momento no tiene nadie detrás

    // Si la cola estaba vacía, el nuevo nodo es el primero y el último a la vez
    if(is_empty(q)) {
        q->front = new_node;
        q->rear = new_node;
    }else{
        // Si ya había elementos, lo enganchamos al final del todo
        q->rear->next = new_node;
        q->rear = new_node; // El nuevo nodo pasa a ser el último
    }
}

// Saca el primer camino de la cola (por el Front) y lo devuelve
Path dequeue_path(Queue* q){
    // Si la cola está vacía, preparamos un camino vacío para devolverlo
    Path empty_path;
    empty_path.count = 0; 
    
    if(is_empty(q)){
        return empty_path;
    }

    // Guardamos temporalmente el nodo que vamos a sacar
    QueueNode* temp = q->front; // para liberarlo despues
    Path p = temp->camino; // Copiamos el camino que tiene dentro

    // Avanzamos el inicio de la cola al siguiente elemento
    q->front = q->front->next;
    
    // Si al avanzar la cola se ha quedado vacía, el rear también debe ser NULL
    if(q->front == NULL){
        q->rear = NULL;
    }
    
    free(temp); // Liberamos la memoria del nodo que hemos sacado de la cola
    return p;   // Devolvemos el camino extraído
}

// Vacía por completo la cola por si se ha quedado algún nodo dentro
void free_queue(Queue* q){
    QueueNode* temp;
    while(q->front != NULL){
        temp = q->front;
        q->front = q->front->next;
        free(temp);
    }
    q->rear = NULL;
}