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

void agregar_bomba (bomba** lista_bombas, int coord1, int coord2, int potencia, int radio) {

    if (lista_bombas == NULL) {
        *lista_bombas = nueva_bomba(coord1, coord2, potencia, radio);
    }
    else {
        bomba *bomba_nueva = nueva_bomba(coord1, coord2, potencia, radio);
        bomba_nueva -> siguiente = *lista_bombas;
        *lista_bombas = bomba_nueva;
    }

}

void imprimir_bombas(bomba *bombas) {
    bomba *aux = bombas;
    while (aux != NULL)
    {
        printf("bomba: %i,%i,%i,%i\n", aux ->coord1,aux->coord2,aux->potencia,aux->radio);
        aux = aux->siguiente;
    }
}

objetivo *nuevo_objetivo(int coord1, int coord2, int resistencia) {
    objetivo *objetivo_nuevo = (objetivo*) malloc(sizeof(objetivo));
    
    if(objetivo_nuevo == NULL){
        printf("ERROR: Reserva de memoria para nuevo objetivo");
		exit(0);	
	}

    objetivo_nuevo -> coord1 = coord1;
    objetivo_nuevo -> coord2 = coord2;
    objetivo_nuevo -> resistencia = resistencia;
    objetivo_nuevo -> siguiente = NULL;

    return objetivo_nuevo;

 }

 void agregar_objetivo (objetivo** lista_objetivos, int coord1, int coord2, int resistencia) {

    if (lista_objetivos == NULL) {
        *lista_objetivos = nuevo_objetivo(coord1, coord2, resistencia);
    }
    else {
        objetivo *objetivo_nuevo = nuevo_objetivo(coord1, coord2,resistencia);
        objetivo_nuevo -> siguiente = *lista_objetivos;
        *lista_objetivos = objetivo_nuevo;
    }

}

void imprimir_objetivos(objetivo *objetivos) {
    objetivo *aux = objetivos;
    while (aux != NULL)
    {
        printf("objetivo: %i,%i,%i\n", aux ->coord1,aux->coord2,aux->resistencia);
        aux = aux->siguiente;
    }
}