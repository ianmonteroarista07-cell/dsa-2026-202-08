#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "house.h"

int levenshtein_distance(const char *s1, const char *s2){
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    
    int matriz[len1+1][len2 +1];
    for (int i = 0; i <= len1; i++){
        matriz[i][0] = i;
    }
    for (int j = 0; j <= len2; j++){
        matriz[0][j] = j;
    }

    for (int i = 1; i <= len1; i++){
        for (int j = 1; j<= len2; j++){
            char char1 = s1[i - 1];
            char char2 = s2[j - 1];

            if(char1 >= 'A' && char1 <= 'Z'){
                char1 = char1 + 32;
            }
            if(char2 >= 'A' && char2 <= 'Z'){
                char2 = char2 + 32;
            }

            int costo;
            if (char1 == char2){
                costo = 0;
            } else{
                costo = 1;
            }

            int elimina_costo = matriz[i - 1][j] + 1;
            int inserta_costo = matriz[i][j - 1] + 1;
            int sustituye_costo = matriz[i - 1][j - 1] + costo;
            int costo_min;

            if (elimina_costo < inserta_costo){
                costo_min = elimina_costo;
            } else {
                costo_min = inserta_costo;
            }

            if (costo_min < sustituye_costo){
                matriz[i][j] = costo_min;
            } else {
                matriz[i][j] = sustituye_costo;
            }

        }
    }

    return matriz[len1][len2];
}