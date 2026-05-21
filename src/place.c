#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> 
#include "place.h"

Place* load_places(char* map_name) {
    char path[100];
    
    // Construimos la ruta dinámica uniendo la carpeta del mapa y el nombre del archivo
    sprintf(path, "maps/%s/places.txt", map_name); 

    // Abrimos el archivo en modo lectura
    FILE* f = fopen(path, "r");
    if(f == NULL) return NULL; //si el archivo no existe, salimos

    Place* head = NULL; // Puntero que siempre indicará el inicio de la lista
    char line[500];
    int count = 0;

    // Aqui torna a passar el mateix problema que abans, on no es carregava els 'places' totals, ja que a la
    // posició 6081 el 'line_type' te comes per dins llavors haurem de cambiar el fscanf per un fgets.
    while(fgets(line, sizeof(line), f)){
        // Variables temporales que guardan lo que se lee en cada línea
        char useless_id[300];  
        char line_name[300];
        char line_type[300];
        double line_lat, line_lon;
        int res;

        /* El ID y el Nombre nunca llevan comillas en este archivo, pero el TIPO sí.
           Para saber si el tipo tiene comillas, usamos un truco con sscanf:
           Intentamos leer primero asumiendo que el tipo SÍ lleva comillas.
           Formato del tipo con comillas: \"%[^\"]\" (lee hasta la comilla de cierre).
        */

        res = sscanf(line, " %[^,],%[^,],\"%[^\"]\",%lf,%lf", useless_id, line_name, line_type, &line_lat, &line_lon);
        
        // Si no ha leído 5 campos, significa que el tipo NO tenía comillas (entonces ha fallado el formato)
        if(res != 5){
            // Volvemos a intentar con el formato normal separado solo por comas (%[^,])
            res = sscanf(line, " %[^,],%[^,],%[^,],%lf,%lf", useless_id, line_name, line_type, &line_lat, &line_lon);
        }
        // Si cualquiera de las dos opciones ha sacado 5 correctamente, entonces:
        if(res == 5){
            //Reservamos memoria dinámica en el Heap
            Place* new_node = malloc(sizeof(Place)); 
            if(new_node == NULL) break; // Seguridad por si falla el malloc
            
            //Traspasamos los datos de las variables temporales a la estructura del nodo
            strcpy(new_node->name, line_name);
            strcpy(new_node->type, line_type);
            new_node->lat = line_lat;
            new_node->lon = line_lon;
            
            //Conectamos el nodo a la lista y hacemos que el 'next' de este nuevo nodo apunte al que antes era el primero
            new_node->next = head;
            //Se actualiza el head para que este nuevo nodo sea el inicio de la lista
            head = new_node;
            
            count++;
        }
    }
    fclose(f);
    printf("%d places loaded\n", count); 
    
    return head; 
}

// void find_place_coordinates(Place* head, char* place_name);
// Antes la función era void, porque solo printeaba la latitud y longitud de los lugares
// Pero ahora va a retornar un tipo Place para guardarme ese nodo con informacion que utilizaré en otras funciones del lab 4.


Place* find_place_coordinates(Place* head, char* place_name){
    //Creamos un puntero para recorrer la lista sin perder el inicio 
    Place* actual = head;

    printf("\n\tBuscando coincidencias para: '%s'...\n", place_name);
    
    //Recorremos la lista nodo a nodo hasta llegar al final
    while(actual != NULL){
        // Buscamos si "place_name" está dentro del NOMBRE del lugar
        char* in_name = strcasestr(actual->name, place_name);
        // Buscamos si "place_name" está dentro del TIPO del lugar
        char* in_type = strcasestr(actual->type, place_name);

        // Si se encuentra en el nombre O en el tipo 
        if(in_name != NULL || in_type != NULL){

            printf("\n\tEncontrado en (%lf, %lf)\n", actual->lat, actual->lon);

            printf("\t\t[%s] - %s\n", actual->type, actual->name);
            return actual; //antes solo habia return;
        }
    
        // Pasamos al siguiente nodo de la lista
        actual = actual->next;
    }
    printf("\tLugar o tipo no encontrado.\n");
    return NULL;
}

void free_places(Place* head){
    Place* temp; //Puntero temporal
    
    while(head != NULL){
        //Guardamos el nodo actual en temp
        temp = head;        
        //Avanzamos head al siguiente nodo antes de borrar el actual porque si se borra 'head' directamente se pierde la conexión al resto de la lista.
        head = head->next;  
        free(temp);         
    }
}