#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>
#include "hashmap.h"
#include <math.h>

/**
 * Inicializa la estructura de la HashTable configurando su tamaño dependiendo de cada mapa
 * y reservado la memoria dinámica necesaria para los buckets.
 */
void init_table(HashTable* table, char* map_name){
    if(table == NULL || map_name == NULL) return;

    // Asignamos el tamaño según el mapa elegido
    if(strcmp(map_name, "2xl_1") == 0){
        table->size = 70009;
    }else if(strcmp(map_name, "xl_1") == 0){
        table->size = 26513;
    }else if(strcmp(map_name, "lg_1") == 0){
        table->size = 5303;
    }else if(strcmp(map_name, "md_1") == 0){
        table->size = 1861;
    }else if(strcmp(map_name, "xs_1") == 0){
        table->size = 101;
    }else if(strcmp(map_name, "xs_2") == 0){
        table->size = 19;
    }

    // Reservamos memoria para el array de punteros simples ( buckets )
    table->buckets = malloc(sizeof(HashNode*) * table->size);
    if(table->buckets == NULL){
        table->size = 0;
        return;
    }
    // Limpiamos los cajones poniéndolos todos a NULL
    for(int i = 0; i < table->size; i++){
        table->buckets[i] = NULL;
    }
}
// Función hash matemática para calcular el índice
int hash_function(long long id, int table_size){
    // Aplicamos modulo (%) para asegurar que el índice esté entre 0 y (table_size - 1)    
    return (int)(id % table_size);
}

// Inserta un tramo del street en la lista enlazada interna del cajón correspondiente
void insert_table(HashTable* table, long long id, Street* segment) {
    if(table == NULL || table->buckets == NULL || segment == NULL) return;

    // Calculamos el cajón usando nuestra función hash
    int index = hash_function(id, table->size);

    // Creamos el nodo intermedio para meterlo en la tabla hash
    HashNode* new_node = malloc(sizeof(HashNode));
    if(new_node == NULL) return;

    // Guardamos la dirección del segmento original
    new_node->segment = segment;

    // Insertamos al principio de la lista de ese bucket
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
}

/**
 * Clasifica cada tramo de la lista unicamente con el nodo de inicio id1
 */
void full_table(HashTable* table, Street* street_list){
    if(table == NULL || street_list == NULL) return;

    Street* current = street_list;
    int count = 0;

    while(current != NULL){
        // Metemos la calle en el cajón de su esquina de inicio (id1)
        insert_table(table, current->id1, current);

        count++;
        current = current->next;
    }
    printf("%d streets han sido indexadas dentro de Hashtable (%d buckets)\n", count, table->size);
}
/**
 * Algoritmo del Lab 5 optimizado con HashTable para buscar conexiones de vías.
 * Respeta la direccionalidad 
 */

void print_connected_streets_hash(HashTable* table, Street* start_segment){
    if(table == NULL || start_segment == NULL) return;

    printf("\tClosest street: %s\n", start_segment->name);
    printf("\tBetween %lld (%lf, %lf) and %lld (%lf, %lf)\n", start_segment->id1, start_segment->p1.lat, start_segment->p1.lon, 
           start_segment->id2, start_segment->p2.lat, start_segment->p2.lon);
    
    Street* current_seg = start_segment;
    int found_next = 1;
    // Para ver si hemos encontrado una conexion real en todo el camino
    int total_connections = 0;

    // Array para controlar los duplicados de nombres en el cruce (máximo 50)
    char printed_streets[50][200];
    int printed_count = 0;
    int cabecera_printed = 0;

    while(found_next){
        found_next = 0;

        // Guardaremos temporalmente el siguiente tramo recto si lo encontramos,
        // pero NO saltaremos a él hasta haber revisado TODO el cajón actual.
        Street* next_recto_seg = NULL; //MODIFICADO // TEMPORAL //ya no es temporal

        // En vez de buscar en toda la lista de calles desde 'head', 50.000 calles,
        // calculamos directos al indice del cajon exacto de la esquina final actual (current_seg->id2)
        int index = hash_function(current_seg->id2, table->size);
        
        // Apuntamos al primer nodo de la lista enlazada de ese cajón específico
        HashNode* p = table->buckets[index]; 

        // Recorremos ÚNICAMENTE los pocos nodos enlazados que colisionaron en este bucket (normalmente entre 1 y 4)
        while(p != NULL){
            
            Street* street_in_bucket = p->segment;
            // PRINTF DE DEPURACIÓN (Muestra qué calle está mirando la HashTable en la RAM)
            //printf("[DEBUG] Buscando en Bucket: Mirando '%s' (id1: %lld -> id2: %lld)\n", //para hacer debug
            //       street_in_bucket->name, street_in_bucket->id1, street_in_bucket->id2);

            // Verificamos si realmente conecta con nuestra esquina
            // El inicio (id1) de la nueva calle analizada 
            // debe nacer exactamente donde termina (id2) nuestro segmento actual.
            if(street_in_bucket->id1 == current_seg->id2){
                // esto es basicamente lo mismo que la otra funcion de print_connected_streets

                // CASO A: Es la misma calle (saltamos por ella)
                if(strcmp(street_in_bucket->name, current_seg->name) == 0){
                    if(street_in_bucket->id2 != current_seg->id1){ // Evitamos bucles hacia atrás
                        //current_seg = street_in_bucket; MODIFICADO //TEMPORAL //ya no es temporal
                        next_recto_seg = street_in_bucket; // Guardamos el tramo para el próximo salto 
                        found_next = 1;  
                        //break; // Reiniciamos el bucle while(found_next) con el nuevo segmento
                        // ¡YA NO HAY BREAK! Seguimos revisando el resto del cajón MODIFICADO //ya no es temporal

                    }
                // CASO B: Es una calle diferente (un cruce)
                }else{
                    // Comprobamos si ya la habíamos impreso en este cruce
                    int already_printed = 0;
                    for(int i = 0; i < printed_count; i++){
                        if(strcmp(street_in_bucket->name, printed_streets[i]) == 0){
                            already_printed = 1;
                            break;
                        }
                    }

                    // Si no está repetida, la guardamos e imprimimos
                    if(already_printed == 0){
                        strcpy(printed_streets[printed_count], street_in_bucket->name);
                        printed_count++;
                        
                        if(cabecera_printed == 0){
                            printf("\n\tFrom this street segment, you can go to:\n");
                            printf("\t- %s\n", start_segment->name);
                            printf("\t\tWhich is connected to:\n");
                            cabecera_printed = 1;
                        }
                        
                        printf("\t\t\t- %s\n", street_in_bucket->name);
                        total_connections++;
                    }
                }
            }
            p = p->next; // Pasamos al siguiente nodo del cajón (Iteramos al siguiente HashNode asignado a este mismo bucket)
        }
        // Una vez que el bucle while(p != NULL) ha terminado de revisar todo el cajón
        // y ha impreso todos los cruces posibles de esta esquina, actualizamos el tramo actual.
        if(found_next && next_recto_seg != NULL){
            current_seg = next_recto_seg;
        }
    }       

    if(total_connections == 0){
        printf("\t\t(No se han encontrado conexiones salientes o es un callejón sin salida)\n");
    }
}

// Libera los nodos creados en el lab5 y el array de buckets
void free_table(HashTable* table){
    if(table == NULL) return;

    // Recorremos cada una de las posiciones del array de buckets
    for(int i = 0; i < table->size; i++){
        HashNode* current = table->buckets[i];
        // Vaciamos secuencialmente la lista enlazada de colisiones del bucket actual
        while(current != NULL){
            HashNode* temp = current;
            current = current->next;
            free(temp); // Liberamos el nodo del Lab 5 
        }
    }
    // Liberamos el bloque contenedor general de punteros (el array buckets)
    free(table->buckets);
    table->buckets = NULL;
    table->size = 0;
}