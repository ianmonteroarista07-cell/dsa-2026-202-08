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
    char line[500]; // Buffer para leer la linea completa del archivo

    /* 
       Usamos fgets para leer la linea entera. 
       Esto evita que el programa se detenga si encuentra una coma 
       inesperada dentro del nombre de una calle.
    */

    while(fgets(line, sizeof(line), f)){
        // sscanf extrae los datos del string "line" que acabamos de leer
        // El formato %[^\n\r] lee todo el texto hasta el final de la línea
        int res = sscanf(line, "%lld,%lf,%lf,%lld,%lf,%lf,%lf,%[^\n\r]", &id1, &lat1, &lon1, &id2, &lat2, &lon2, &dist, street_name);
        // Si hemos leído al menos los 7 datos numéricos, la línea es válida
        if(res >= 7){
            // Creamos un nuevo nodo para nuestra lista
            Street *new_node = malloc(sizeof(Street));
            if (new_node == NULL) break;    // Seguridad por si falla el malloc
            
            // Guardamos los números que acabamos de leer en el nuevo nodo
            new_node->id1 = id1;
            new_node->p1.lat = lat1;
            new_node->p1.lon = lon1;
            new_node->id2 = id2;
            new_node->p2.lat = lat2;
            new_node->p2.lon = lon2;
            new_node->length = dist;

            // Si res es 8, significa que sscanf encontró un nombre
            if(res == 8){
                strcpy(new_node->name, street_name);
            }else{
                // Si res es 7, el nombre estaba vacío en el archivo
                strcpy(new_node->name, "Unknown Street");
            }
            // Metemos el nuevo nodo al principio de la lista (como siempre)
            new_node->next = head;
            head = new_node;
            count++;    // Contamos cada calle
        }
    }
    // Cerramos el archivo y avisamos de cuantas calles hemos cargado
    fclose(f);
    printf("%d streets loaded\n", count);
    return head; // Devolvemos la lista completa
}
// He cambiado el load_streets porque antes solo leia 18849 de las 18944 que habian en streets.txt
// Ahora carga todas las streets

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
// Muestra a que calles puedes ir desde la calle actual
void print_connected_streets(Street* head, Street* start_segment){
    if(start_segment == NULL) return;

    printf("\tClosest street: %s\n", start_segment->name);
    printf("\tBeetween %lld (%lf, %lf) and %lld (%lf, %lf)\n", start_segment->id1, start_segment->p1.lat, start_segment->p1.lon, start_segment->id2, start_segment->p2.lat, start_segment->p2.lon);
    
    // 'current_seg' és el segment que estem analitzant en cada salt. 
    // Comencem pel segment més proper (start_segment).
    Street *current_seg = start_segment;
    // 'found_next' actua com un senyal per saber si hem de seguir avançant per la mateixa carrer
    int found_next = 1;
    // Comptador per saber si hem trobat alguna connexió real al final de tot el camí
    int total_connections = 0;
    // Creamos un array para guardar hasta 50 nombres de calles diferentes en el cruce
    char printed_streets[50][200];
    int printed_count = 0;

    //control de si hemos imprimido la cabecera
    int cabecera_printed = 0;

    while(found_next){
        found_next = 0;
        Street *p = head; // Puntero auxiliar para recorrer la lista desde el inicio
        while(p != NULL){
            // ¿Conecta el inicio de 'p' con el final de nuestro tramo actual?
            // Condición de DIRECCIONALIDAD:
            // Solo nos interesan los segmentos que COMIENCEN (id1) 
            // donde ACABA el nuestro (current_seg->id2).
            if(p->id1 == current_seg->id2){
                // CASO A: Misma calle (Seguimos saltando)
                /* 
                TROBAR UN CARRER DIFERENT:
                Hem de trobar un carrer diferenten el que estem.
                Això ho fem comparant els noms amb strcmp.
                */
                if(strcmp(p->name, current_seg->name) == 0){
                    /* 
                    VIGILAR BUCLES (Carrers bidireccionals):
                    No volem entrar en bucles llavors si el segment que hem trobat
                    torna cap enrere (acaba on nosaltres hem començat), l'ignorem.
                    */
                    if(p->id2 != current_seg->id1){ // Evitamos volver atrás
                        current_seg = p; 
                        found_next = 1;  
                        break; // Reiniciamos búsqueda desde el principio con el nuevo tramo
                    }
                }else{

                    // CASO B: Calle distinta (Cruce encontrado)
                    // COMPROBAR SI YA HEMOS IMPRESO ESTA CALLE
                    int already_printed = 0;
                    for(int i = 0; i < printed_count; i++){
                        if(strcmp(p->name, printed_streets[i]) == 0){
                            already_printed = 1; // Al cambiar a 1 significa que la calle ya esta repetida
                            break; // Entonces no hace falta seguir buscando en la lista y salimos del for con un break
                        }
                    }
                    // SI no hemos impreso la calle entonces: 
                    if(already_printed == 0){
                        strcpy(printed_streets[printed_count], p->name); // lo ponemos en la lista en la posicion primera
                        printed_count++; // y movemos la 'i' de la lista para que apunte a la siguiente posicion
                        
                        // Ahora vamos a hacer que la cabecera solo la imprima una vez y no se repita
                        if(cabecera_printed == 0){
                            printf("\n\tFrom this street segment, you can go to:\n");
                            printf("\t- %s\n", start_segment->name);
                            printf("\t\tWhich is connected to:\n");
                            cabecera_printed = 1; // Bloqueamos para que no vuelva a entrar aqui
                            // hacemos esto para que no se repita mas veces la misma frase
                        }
                        // Imprimimos únicamente el nombre de la calle conectada
                        printf("\t\t\t- %s\n", p->name);
                        total_connections++;
                        // No ponemos found_next = 1 para no seguir saltando más allá del cruce
                    }
                }
            }
            p = p->next;
        }
    }       
    // Si después de todos los saltos el contador sigue en 0, avisamos
    if(total_connections == 0){
        printf("\t\t(No se han encontrado conexiones salientes o es un callejón sin salida)\n");
    }
}
/* * EXPLICACIÓN DE LA LÓGICA DE INTERSECCIONES:
 * 1. El bucle externo (while found_next) permite "saltar" entre segmentos 
 * que tienen el mismo nombre (la misma calle continúa).
 * * 2. El bucle interno (while p != NULL) recorre TODA la lista de calles 
 * para el nodo actual (current_seg->id2).
 * * 3. ¿Por qué imprime más de una conexión? 
 * Si en un cruce hay 3 calles diferentes (ej: Rambla y Ronda), el bucle 
 * interno las encontrará todas a medida que avanza por la lista y las 
 * imprimirá una a una. 
 * * 4. ¿Por qué no usamos 'break' al encontrar una calle distinta?
 * Si pusiéramos un 'break' tras el printf de una calle distinta, el 
 * programa se detendría en la primera que encontrara y no veríamos 
 * las demás opciones del cruce. Solo usamos 'break' cuando es la MISMA 
 * CALLE (mismo nombre) para actualizar nuestra posición y volver a 
 * empezar la búsqueda desde el nuevo final.
 */

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