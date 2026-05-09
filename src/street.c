#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define  _USE_MATH_DEFINES
#include <math.h> 
#include "street.h"
#define EARTH_RADIUS 6371.0

// --- FUNCIONES DE CONVERSIÓN ---

// Pasa de grados a radianes 
double toRadians(double grados){
    return grados * (M_PI/180);
}
// Pasa de radianes de vuelta a grados decimales
double toDegrees(double radians) {
    return radians * (180.0 / M_PI);
}

// Encuentra el punto exacto en la mitad de un segmento de calle
Position midpoint(Position a, Position b){
    double lat1 = toRadians(a.lat);
    double lon1 = toRadians(a.lon);
    double lat2 = toRadians(b.lat);
    double lon2 = toRadians(b.lon);

    double x1 = cos(lat1) * cos(lon1);
    double y1 = cos(lat1) * sin(lon1);
    double z1 = sin(lat1);

    double x2 = cos(lat2) * cos(lon2);
    double y2 = cos(lat2) * sin(lon2);
    double z2 = sin(lat2);

    double x = (x1 + x2) / 2.0;
    double y = (y1 + y2) / 2.0;
    double z = (z1 + z2) / 2.0;

    double lon = atan2(y, x);
    double hyp = sqrt(x * x + y * y);
    double lat = atan2(z, hyp);

    Position mid;
    mid.lat = toDegrees(lat);
    mid.lon = toDegrees(lon);
    return mid;
}

// Lee el archivo "streets.txt" y crea la lista enlazada de calles
Street* load_streets(char* map_name){
    char path[100];
    // Creamos la ruta al archivo
    sprintf(path, "maps/%s/streets.txt", map_name);

    // Intentamos abrir el archivo en modo lectura ("r") 
    FILE* f = fopen(path, "r");
    if(f == NULL) return NULL; // Si no existe el mapa, salimos

    Street* head = NULL; // El "puntero" al principio de nuestra lista
    int count = 0;  // Contador para saber cuantas calles cargamos

    // Variables donde guardaremos temporalmente lo que leamos de cada linea
    long long id1, id2;
    double lat1, lat2, lon1, lon2, dist;
    char street_name[250];

    /* 
       EL BUCLE PRINCIPAL:
       Leemos los 7 datos numéricos que SIEMPRE estan: id1, lat1, lon1, id2, lat2, lon2 y dist.
       Ponemos una coma al final del formato "%lf," para que el programa se coma la coma que 
       separa el numero del nombre de la calle.
    */

    // Antes tambien habia uno para el nombre de la calle, pero hay algunas lineas que no tienen nombre de calle
    // antes habia street_name) == 8

    while(fscanf(f, " %lld,%lf,%lf,%lld,%lf,%lf,%lf,", &id1, &lat1, &lon1, &id2, &lat2, &lon2, &dist) == 7){
        // Creamos un nuevo nodo para nuestra lista
        Street *new_node = malloc(sizeof(Street));  
        if(new_node == NULL) break; // Seguridad por si falla el malloc

        // Guardamos los números que acabamos de leer en el nuevo nodo
        new_node->id1 = id1;
        new_node->p1.lat = lat1;
        new_node->p1.lon = lon1;
        new_node->id2 = id2;
        new_node->p2.lat = lat2;
        new_node->p2.lon = lon2;
        new_node->length = dist;
        // strcpy(new_node->name, street_name); Antes habia esto,

        // Ahora habrá esto porque, evaluamos ambos casos donde hay nombre de calle y donde no
        /* 
           LECTURA DEL NOMBRE:
           Intentamos leer el nombre de la calle hasta el final de la linea (%[^\n\r]).
           - Si fscanf devuelve 1: Significa que ha encontrado un nombre y lo guarda.
           - Si NO devuelve 1: Significa que la linea se acabó despues de la ultima coma 
             (como en la línea 205 del xl_1). En ese caso, le ponemos "Unknown Street".
        */
        if(fscanf(f, " %[^\n\r]", street_name) == 1){
            strcpy(new_node->name, street_name);
        }else{
            strcpy(new_node->name, "Unknown Street");
        }
        // Metemos el nuevo nodo al principio de la lista (como siempre)
        new_node->next = head;
        head = new_node;

        count++;    // Contamos cada calle
    }
    // Cerramos el archivo y avisamos de cuantas calles hemos cargado
    fclose(f);
    printf("%d streets loaded\n", count);
    return head; // Devolvemos la lista completa
}

// Libera la memoria
void free_streets(Street* head){
    Street* temp;
    while(head != NULL){
        temp = head;

        head = head->next;
        free(temp);
    }
}
// Busca que segmento de calle es el mas cercano a donde esta el usuario
Street* get_closest_street(Street *head, Position userPos){
    Street* current = head;
    Street* closest = NULL;
    double min_dist = 100000.0000000; // Distancia inicial muy grande

    while(current != NULL){
        // Calculamos el punto medio del segmento actual
        Position mid = midpoint(current->p1, current->p2);
        // Calculamos distancia real entre usuario y ese punto medio
        double d = haversine(userPos, mid);

        if(d < min_dist){
            min_dist = d;
            closest = current;
        }
        current = current->next;
    }
    return closest;
}
// Muestra a quw calles puedes ir desde la calle actual
void print_connected_streets(Street* head, Street* closest){
    if(closest == NULL) return;

    printf("\tClosest street: %s\n", closest->name);
    printf("\tBeetween %lld (%lf, %lf) and %lld (%lf, %lf)\n", closest->id1, closest->p1.lat, closest->p1.lon, closest->id2, closest->p2.lat, closest->p2.lon);
    
    printf("\n\tFrom this street segment, you can go to:\n");

    Street *current = head;

    while(current != NULL){
        // No comparamos el mismo segmento (es decir contigo mismo)
        if(current != closest){
            // El inicio de la siguiente es el final de la actual
            if(current->id1 == closest->id2){
                printf("DEBUG: He trobat un segment que comença al node %lld\n", current->id1);
                // Evitamos el giro de 180 grados (volver por donde vinimos)
                if(current->id2 != current->id1){ // strcmp(current->name, closest->name) != 0  no se que poner de estos dos (PREGUNTAR) // current->id2 != current->id1
                    printf("\t- %s\n", closest->name);
                    printf("\t\tWhich is connected to:\n");
                    printf("\t\t\t- %s\n", current->name);
                }
            }
        }
        current = current->next;
    }
}
// Formula de Haversine para calcular distancia en curva sobre la Tierra
double haversine(Position posA, Position posB){
    double lat1 = toRadians(posA.lat);
    double lon1 = toRadians(posA.lon);
    double lat2 = toRadians(posB.lat);
    double lon2 = toRadians(posB.lon);

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    double a = pow(sin(dLat/2), 2) + cos(lat1) * cos(lat2) * pow(sin(dLon/2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS * c;


}