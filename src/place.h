#ifndef PLACE_H
#define PLACE_H

typedef struct Place{
    char name[100];
    char type[100]; 
    double lat;
    double lon;
    struct Place* next;
} Place;

Place* load_places(char* map_name);
void find_place_coordinates(Place* head, char* place_name);
void free_places(Place* head);

#endif 