#include <stdio.h>
#define  _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "route_math.h"
#define EARTH_RADIUS 6371.0
#include "street.h"



// Transforma la latitud y longitud a metros (coordenadas X e Y)
void latlon_to_xy(double lat_ref, double lon_ref, double lat, double lon, double *x, double *y){
    double lat_ref_rad = toRadians(lat_ref);
    double dlat = toRadians(lat - lat_ref);
    double dlon = toRadians(lon - lon_ref);
    
    // Calculamos los metros usando las fórmulas que te ha dado el profesor
    *x = EARTH_RADIUS * dlon * cos(lat_ref_rad) * 1000.0; 
    *y = EARTH_RADIUS * dlat * 1000.0;
}

// Analiza si para pasar de la calle 'a' a la calle 'b' hay que girar a la izquierda o derecha
int get_turn_direction(Street* a, Street* b){
    // Usamos el primer punto de la calle 'a' como nuestro origen (0,0) de referencia
    double lat_ref = a->p1.lat;
    double lon_ref = a->p1.lon;

    double ax, ay, bx, by, cx, cy;

    // Convertimos todos los puntos geométricos a metros X e Y
    latlon_to_xy(lat_ref, lon_ref, a->p1.lat, a->p1.lon, &ax, &ay); // Convertimos el punto de referencia: debe quedar en (0,0)
    latlon_to_xy(lat_ref, lon_ref, a->p2.lat, a->p2.lon, &bx, &by); // Fin de calle A e inicio de B
    latlon_to_xy(lat_ref, lon_ref, b->p2.lat, b->p2.lon, &cx, &cy); // Fin de calle B

    // Fórmula del Producto Vectorial explicada en tu enunciado:
    // (Bx - Ax) * (Cy - By) - (By - Ay) * (Cx - Bx)
    double cross_product = (bx - ax) * (cy - by) - (by - ay) * (cx - bx);

    // Si el resultado es positivo, el giro es a la izquierda (Turn left)
    if(cross_product > 0){
        return 1;  
    } 
    // Si el resultado es negativo, el giro es a la derecha (Turn right)
    else if(cross_product < 0){
        return -1; 
    }
    
    // Si es prácticamente 0, es que seguimos recto
    return 0;
}