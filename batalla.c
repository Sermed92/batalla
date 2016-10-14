#include <stdio.h>
#include <stdlib.h>
#include "batalla.h"


 bomba *nueva_bomba(int coord1, int coord2, int potencia, int radio) {
     bomba *bomba_nueva = (bomba*) malloc(sizeof(bomba));
     
     if(bomba_nueva == NULL){
		printf("ERROR: Reserva de memoria para nueva bomba");
		exit(0);	
	}

    bomba_nueva -> coord1 = coord1;
    bomba_nueva -> coord2 = coord2;
    bomba_nueva -> potencia = potencia;
    bomba_nueva -> radio = radio;
    bomba_nueva -> siguiente = NULL;

    return bomba_nueva;

 }

void agregar_bomba (bomba* lista_bombas, int coord1, int coord2, int potencia, int radio) {

    bomba *bomba_nueva = nueva_bomba(coord1, coord2, potencia, radio);
    bomba_nueva -> siguiente = lista_bombas;
    lista_bombas = bomba_nueva;

}