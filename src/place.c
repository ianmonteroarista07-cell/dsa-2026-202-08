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
    
    // Variables temporales que guardan lo que se lee en cada línea
    char line_name[100];
    char line_type[100];
    double line_lat, line_lon;
    int count = 0;

    char header[200];
    if (fgets(header, sizeof(header), f) == NULL) { // Si el archivo está vacío se sale
        fclose(f);
        return NULL; 
    }
    //Bucle para leer el archivo línea a línea.
    //El espacio inicial ignora saltos de línea o espacios en blanco previos.
    //%99[^,] lee hasta 99 caracteres, pero se detiene en cuanto ve una coma (,).
    //Si fscanf logra leer las 4 variables correctamente, devuelve 4 y el bucle sigue.
    while(fscanf(f, " %99[^,],%99[^,],%lf,%lf\n", line_name, line_type, &line_lat, &line_lon) == 4){
        
        //Reservamos memoria dinámica en el Heap
        Place* new_node = malloc(sizeof(Place)); 
        
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

    fclose(f);
    printf("%d places loaded\n", count); 
    
    return head; 
}

void find_place_coordinates(Place* head, char* place_name){
    //Creamos un puntero para recorrer la lista sin perder el inicio 
    Place* actual = head;
    int found = 0; //Para saber si hemos tenido éxito en la búsqueda

    //Recorremos la lista nodo a nodo hasta llegar al final
    while(actual != NULL){
        
        // strcasecmp compara textos ignorando si son mayúsculas o minúsculas y devuelve 0 si ambas cadenas son exactamente iguales 
        if(strcasecmp(place_name, actual->name) == 0){
            printf("\n\tLugar '%s' encontrado en (%lf,%lf). Tipo: %s\n", 
                   actual->name, actual->lat, actual->lon, actual->type);
            found = 1; //Marcamos lo encontrado
            return;
        }
    
        // Si no es lo buscado, avanzamos al siguiente nodo
        actual = actual->next;
    }

    // Si el bucle termina y found sigue siendo 0, el lugar no estaba en la lista
    if(!found){
        printf("\n\tLugar no encontrado.\n");
    }
}

void free_places(Place* head){
    Place* temp; //Puntero temporal
    
    while(head != NULL){
        //Guardamos el nodo actual en temp
        temp = head;        
        //Avanzamos head al siguiente nodo antes de borrar el actual porque i se borra 'head' directamente se pierde la conexión al resto de la lista.
        head = head->next;  
        free(temp);         
    }
}