#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "house.h"


// Funcion para pedir repetidas veces el nombre del mapa hasta que dé con uno correcto
// que esté en la lista permitida.
void get_map_name(char* map_name){

    char *valido[] = {"2xl_1","lg_1","md_1","xl_1","xs_1","xs_2"};
    printf("Introduzca el nombre del mapa (e.g. 'xs_2' o 'xl_1'): ");
    scanf("%s", map_name);
    int ok = 0;

    // No sale hasta que el mapa este correcto
    while(ok == 0){
        ok = 0;
        for(int i = 0; i < 6; i++){
            if(strcmp(map_name,valido[i]) == 0){ //Comparamos cada nombre valido de la lista con el nombre dado
                ok = 1;
                break;
            }
        }
        if(ok == 0){
            printf("[ERROR] Nombre invalido, intentalo de nuevo: ");
            scanf("%s", map_name);
        }
    }
    printf("Mapa válido: %s\n", map_name);
}

// Funcion para contar lineas (es temporal, para cargar los "places" y "streets")
/*
   Abre un archivo de texto y cuenta cuantos saltos de línea tiene.
   Se usa para saber cuántas calles o sitios hay cargados.
*/
int count_lines(char* map_name, char* file_name) {
    char archivo[100];
    // sprintf guarda el resultado en una variable
    // en lugar de mostrarlo por pantalla. Aquí la usamos para construir la ruta 
    // hacia el archivo: "maps/" + nombre_mapa + "/" + nombre_archivo.
    sprintf(archivo, "maps/%s/%s", map_name, file_name);

    FILE* f = fopen(archivo, "r");
    if(f == NULL) return 0;

    int count = 0;
    char c;
    // Leemos caracter a caracter hasta el final del archivo (EOF)
    while((c = fgetc(f)) != EOF){
        if(c == '\n')count++;
    }
    fclose(f);
    return count;
} 

// Para cargar las casas en la lista enlazada
/*
   Carga los datos de houses.txt en una lista enlazada en memoria.
   Devuelve el puntero al primer nodo (head).
*/
House* load_houses(char* map_name) {
    char path[100];
    sprintf(path, "maps/%s/houses.txt", map_name);

    FILE* f = fopen(path, "r");
    if(f == NULL) return NULL;

    House* head = NULL; // El principio de la lista
    char line_street[100];
    int line_num;
    double line_lat, line_lon;
    int count = 0; // Ponemos el contador aqui directamente, y no haria falta utilizar la otra funcion de count_lines

    // Leemos el archivo linea a linea con el formato de (calle,numero,lat,lon)
    // %99[^,] significa lee hasta 99 caracteres o hasta encontrar una coma
    // Esto lo hacemos porque sino solo leeria la primera palabra y el resto nunca lo cogeria
    while(fscanf(f, " %99[^,],%d,%lf,%lf\n", line_street, &line_num, &line_lat, &line_lon) == 4){
        
        // Reservamos memoria para un nuevo nodo
        House* new_node = malloc(sizeof(House));
        
        // Metemos los datos dentro del nodo
        strcpy(new_node->street_name, line_street);
        new_node->house_number = line_num;
        new_node->lat = line_lat;
        new_node->lon = line_lon;
        
        // Conectarlo al principio de la lista 
        new_node->next = head;
        head = new_node;
        
        count++;
    }

    fclose(f);
    printf("%d houses loaded\n", count); 
    return head;
}
/*
  Busca una casa por nombre de calle y numero.
  Si la calle existe pero el numero no, muestra los numeros disponibles.
*/

void find_house_coordinates(House* head, char* street, int number){
    // Creamos un puntero auxiliar "actual" que apunta al inicio de la lista (head).
    // Lo usamos para recorrer la lista nodo a nodo, permitiéndonos avanzar con "actual = actual->next".
    House* actual = head;
    int street_exist = 0;
    char search_name[100];

    // Comparamos el caso cuando nos lo dan abreviado
    // Si el usuario escribe "c. ", lo transformamos a "Carrer "
    // strncasecmp compara sin importar mayúsculas las primeras N letras
    if(strncasecmp(street, "c. ", 3) == 0){
            sprintf(search_name, "Carrer %s", street +3);
        }else strcpy(search_name, street);
        
    // Lo que hace el strncasecmp es:
    // Comparara solo las 3 primeras letras, y no importa si son mayusculas o minusculas
    // solo mira 'c' '.' ' '
    // si son iguales da 0.
    // ponemos un +3 para coger lo que haya despues de "c. " porque avanza 3 posiciones

    // Buscar coincidencia exacta de calle y numero
    while(actual != NULL){
        if(strcasecmp(search_name, actual->street_name) == 0){
            street_exist = 1;   // Marcamos que la calle al menos existe
            if(actual->house_number == number){
                printf("\n\tEncontrado en (%lf,%lf)\n", actual->lat, actual->lon);
                return;     // Salimos al encontrarlo
            }
        }
        actual = actual->next;
    }

    // Si la calle existe pero el numero es incorrecto entonces
    if(street_exist){
        printf("\n\tLa calle '%s' existe, pero el numero %d no.\n", street, number);
        printf("\tNumeros validos: ");

        // Reiniciamos el puntero al principio de la lista (para volver a recorrerla)
        actual = head;
        // Recorremos toda la lista otra vez
        while(actual != NULL){
            // Si el nombre de la calle coincide imprimimos el numero
            if(strcasecmp(search_name, actual->street_name) == 0){
                // Imprimimos todos los números asociados a esa calle
                printf("%d ", actual->house_number);
            }

            //Pasamos al siguiente nodo
            actual = actual->next;
        }
        printf("\n\tPor favor prueba con uno de esos de esos numeros.\n");
    }else{
        printf("\n\tDireccion no encontrada.\n");
    }

}
/*
   Pide los datos de la direccion. 
   Usamos %99[^\n] en scanf para permitir espacios en el nombre de la calle.
*/
void get_adress_name(char* street_final, int* num_final){
    printf("Introduzca nombre de la calle (e.g. 'Carrer de Roc Boronat'): ");
    // %[^\n] le dice a scanf que lea todos los caracteres, incluyendo espacios, 
    // y que solo se detenga al encontrar un salto de linea (la tecla ENTER). 
    // Usamos esto en lugar de %[^,] porque el usuario no escribe comas al 
    // escribir la calle, sino que termina la frase pulsando ENTER.
    scanf(" %99[^\n]", street_final);

    printf("Introduzca numero de la calle (e.g. '138'): ");
    if(scanf("%d", num_final) != 1){
        *num_final = -1; //error si no es un numero.
    }

}
/*
   Libera toda la memoria dinamica ocupada por la lista enlazada.
   Es para evitar Memory Leaks.
*/
void free_houses(House* head){
    House* temp;
    while(head != NULL){
        temp = head;        // Guardamos el nodo actual
        head = head->next;  // Saltamos al siguiente antes de borrar nada
        free(temp);         // Liberamos la memoria del nodo guardado
    }
}