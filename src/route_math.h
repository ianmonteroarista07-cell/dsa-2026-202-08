#ifndef ROUTE_MATH_H
#define ROUTE_MATH_H

#include "street.h"

/* 
 * Transforma coordenadas geograficas (Latitud y Longitud) a metros reales (X e Y)
 * tomando como origen matemático un punto de referencia (lat_ref, lon_ref).
 */
void latlon_to_xy(double lat_ref, double lon_ref, double lat, double lon, double *x, double *y);

/* 
 * Compara dos calles consecutivas ('a' y 'b') usando el producto vectorial.
 * Devuelve:
 * 1   -> Si hay que girar a la izquierda
 * -1  -> Si hay que girar a la derecha
 * 0   -> Si se continúa recto
 */
int get_turn_direction(Street* a, Street* b);

#endif