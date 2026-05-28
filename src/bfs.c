#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bfs.h"
#include "queue.h"
#include "route_math.h"


// Función para comprobar si una calle ya está en la lista de visitados
int is_street_visited(Street** visited_list, int count_visited, Street* street){
    // Comparamos los IDs y el nombre para estar seguros de que es la misma calle
    for(int i = 0; i < count_visited; i++){
        if(visited_list[i]->id1 == street->id1 && visited_list[i]->id2 == street->id2 && strcmp(visited_list[i]->name, street->name) == 0){
            return 1; // Ya está visitada
        }
    }
    return 0;  // No visitada
}
void print_route_instructions(Path* path, Street* fromStreet, Street* toStreet) {
    // Si la ruta es nula o no contiene ningún segmento, podemos ver que
    // el origen y el destino son el mismo punto.
    if(path == NULL || path->count == 0){
        printf("  You have arrived to %s\n", toStreet->name);
        return;
    }
    // Mensaje inicial
    printf("  Start at %s\n", fromStreet->name);

    // Guardamos los metros del primer segmento
    double metros_acumulados = path->streets[0]->length;

    // Calculamos el giro del primer tramo de la ruta (comparando el primer segmento 
    // con el segundo) antes de que empiece el bucle.
    int turn_actual = 0;
    if(path->count > 1){
        turn_actual = get_turn_direction(path->streets[0], path->streets[1]);
    }

    // Recorremos la ruta desde el segundo segmento para ir comparando con el anterior
    for(int i = 1; i < path->count; i++){
        Street* antes = path->streets[i - 1];
        Street* ahora = path->streets[i];

        // Si tienen el mismo nombre, seguimos en la misma calle y acumulamos metros
        if(strcmp(antes->name, ahora->name) == 0){
            metros_acumulados += ahora->length;
        }else{
            // Ha cambiado el nombre de la calle 
            // Aplicamos el redondeo e imprimimos la calle de atrás
            int metros = (int)(metros_acumulados + 0.5);
            
            if(turn_actual == 1){
                printf("  Turn left to %s and continue for %dm\n", antes->name, metros);
            }else if(turn_actual == -1){
                printf("  Turn right to %s and continue for %dm\n", antes->name, metros);
            }else{
                printf("  Continue straight to %s and continue for %dm\n", antes->name, metros);
            }

            // Calculamos el giro justo en esta intersección para la nueva calle (el cruce entre 'antes' y 'ahora') 
            turn_actual = get_turn_direction(antes, ahora);

            // Reiniciamos el acumulador con los metros del nuevo tramo
            metros_acumulados = ahora->length;
        }
    }
    
    // Imprimimos el último bloque que se quedó acumulado al salir del bucle
    // Al salir del bucle 'for', la última calle de la ruta nunca se imprime dentro de él
    // porque no hay un segmento posterior que rompa el 'strcmp'. Lo imprimimos de forma manual por aqui
    int metros_finales = (int)(metros_acumulados + 0.5);
    Street* ultima_calle = path->streets[path->count - 1];
    
    if(turn_actual == 1){
        printf("  Turn left to %s and continue for %dm\n", ultima_calle->name, metros_finales);
    }else if (turn_actual == -1){
        printf("  Turn right to %s and continue for %dm\n", ultima_calle->name, metros_finales);
    }else{
        printf("  Continue straight to %s and continue for %dm\n", ultima_calle->name, metros_finales);
    }

    // 2. Mensaje final obligatorio de llegada a destino
    printf("  You have arrived to %s\n", toStreet->name);
}

void find_route_bfs(HashTable* table, Street* fromStreet, Street* toStreet) {

    // Creamos e inicializamos la cola vacía. Almacena rutas enteras (Path)
    Queue Q;
    init_queue(&Q);

    // Creamos la ruta inicial, que de momento solo contiene 1 calle: la de origen
    Path initial_path;
    initial_path.count = 1;
    initial_path.streets[0] = fromStreet;

    // Metemos esta primera ruta (origen) en la cola para empezar a buscar desde ahí
    enqueue_path(&Q, initial_path);

    // Reservamos memoria para el array de streets visitadas 
    // Guardamos las calles por las que ya hemos pasado para no entrar en bucles infinitos
    Street** visited = malloc(50000 * sizeof(Street*)); 
    int visited_count = 0;

    // Variables de control para saber si encontramos camino y almacenar la ruta ganadora
    int path_found = 0;
    Path final_path;
    final_path.count = 0;

    // Mientras queden rutas por explorar en la cola, seguimos buscando
    while(!is_empty(&Q)){
        
        // Sacamos la ruta más antigua de la cola 
        Path current_path = dequeue_path(&Q);
        
        // Averiguamos cuál es la última calle de la ruta que estamos analizando ahora
        Street* current_street = current_path.streets[current_path.count - 1];

        // Es esta última calle la calle de destino?
        // Comparamos los IDs de los extremos y el nombre para estar seguros
        if(current_street->id1 == toStreet->id1 && current_street->id2 == toStreet->id2 && strcmp(current_street->name, toStreet->name) == 0){
            // Si es que si guardamos esta ruta completa como la definitiva y salimos del bucle
            final_path = current_path;
            path_found = 1;
            break; 
        }

        // Si no es el destino, miramos si ya habíamos explorado esta calle antes
        if(!is_street_visited(visited, visited_count, current_street)){
            
            // Si es nueva, la ponemos en el array de visitados
            visited[visited_count] = current_street;
            visited_count++;

            // Averiguamos donde termina nuestra calle actual (id2)
            // Queremos saber qué calles empiezan justo en esa intersección
            long long nodo_buscado = current_street->id2;
            
            // Calculamos la posición (bucket) en la HashTable usando el ID del nodo final
            int bucket = hash_function(nodo_buscado, table->size);
            
            // Recorremos la lista enlazada de calles que hay dentro de ese bucket de la tabla
            HashNode* p = table->buckets[bucket];
            while(p != NULL){
                // Extraemos el tramo de calle que está guardado DENTRO del nodo
                Street* connected_street = p->segment;
            
                // Si la calle del mapa empieza (id1) donde termina la nuestra (nodobuscado), es vecina
                if(connected_street->id1 == nodo_buscado){

                    // Comprobamos que esa calle vecina no la hayamos visitado ya en otra ruta
                    if(!is_street_visited(visited, visited_count, connected_street)){
                        
                        // Clonamos la ruta actual para crear una nueva variante extendida
                        Path new_path = current_path; 
                        
                        // Vemos si no desbordamos el array estático de 500 calles
                        if(new_path.count < 500){ 
                            // Añadimos la calle vecina al final de esta nueva ruta
                            new_path.streets[new_path.count] = connected_street;
                            new_path.count++;
                            
                            // Metemos la nueva ruta extendida en la cola para explorarla más adelante
                            enqueue_path(&Q, new_path);
                        }
                    }
                }
                // Avanzamos al siguiente nodo de la lista enlazada dentro del mismo bucket de la HashTable
                p = p->next; 
            }
        }
    }
    if(path_found){
        /*// ====================================================================
        //  [DEBUG TRACE] IMPRIMIMOS TODOS LOS SEGMENTOS QUE EL BFS ENCONTRÓ
        // ====================================================================
        printf("\n================================================================\n");
        printf("[DEBUG] TOTAL DE SEGMENTOS ENCONTRADOS POR EL BFS: %d\n", final_path.count);
        printf("================================================================\n");
        for(int k = 0; k < final_path.count; k++) {
            Street* s = final_path.streets[k];
            printf("  Segmento [%d] -> Calle: '%s' | Longitud: %.2f m | ID1: %lld -> ID2: %lld\n", 
                   k, s->name, s->length, s->id1, s->id2);
        }
        printf("================================================================\n\n");
        // ====================================================================*/

        // Llamamos a tu función de impresión normal para ver qué saca después
        print_route_instructions(&final_path, fromStreet, toStreet);
    }else{
        // Entra aquí únicamente si la cola se vació por completo y no se encontró ninguna conexión
        printf("  No path found between these positions.\n");
    }
    // Liberamos todos los nodos que se hayan quedado colgados dentro de la cola
    free_queue(&Q);
    
    // Liberamos el array dinámico de calles visitadas que creamos al principio con malloc
    free(visited);
}

