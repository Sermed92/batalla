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
        printf("bomba: %i,%i,p:%i,r:%i\n", aux ->coord1,aux->coord2,aux->potencia,aux->radio);
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

bomba *pop_bomba (bomba** lista_bomba) {
    bomba* primera_bomba = *lista_bomba;
    if ((*lista_bomba) -> siguiente != NULL) {
        *lista_bomba = (*lista_bomba) -> siguiente;
    }
    primera_bomba -> siguiente = NULL;
    return primera_bomba;
}

// Funcion para determinar si un bojetivo esta dentro del rango de accion de una bomba
// retorna 1 si el objetivo esta en rango o 0 en caso contrario
int esta_en_radio (objetivo *objetivo_actual, bomba *bomba_actual) {
    if (bomba_actual -> coord1 - bomba_actual -> radio <= objetivo_actual -> coord1) {
        if (objetivo_actual -> coord1 <= bomba_actual -> coord1 + bomba_actual -> radio) {
            if (bomba_actual -> coord2 - bomba_actual -> radio <= objetivo_actual -> coord2) {
                if (objetivo_actual -> coord1 <= bomba_actual -> coord1 + bomba_actual -> radio) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void procesar_impacto(objetivo **objetivo_actual, bomba *bomba_actual) {
    if ((*objetivo_actual) -> resistencia < 0) {
        (*objetivo_actual) -> resistencia += bomba_actual -> potencia;
        if ((*objetivo_actual) -> resistencia > 0) {
            (*objetivo_actual) -> resistencia = 0;
        }
    }

    if ((*objetivo_actual) -> resistencia > 0) {
        (*objetivo_actual) -> resistencia -= bomba_actual -> potencia;
        if ((*objetivo_actual) -> resistencia < 0) {
            (*objetivo_actual) -> resistencia = 0;
        }
    }
}

void lanzar_bomba(objetivo **objetivos_militares, objetivo **objetivos_civiles, bomba *bomba_actual) {
    objetivo *temp_militar = *objetivos_militares;
    //printf("bomba (%i,%i)\n", bomba_actual -> coord1, bomba_actual-> coord2);
    while (temp_militar != NULL) {
        //printf("Objetivo militar alcanzado %i,(%i,%i)\n", esta_en_radio(temp_militar,bomba_actual),temp_militar -> coord1, temp_militar -> coord2);
        if (esta_en_radio(temp_militar, bomba_actual)) {
            procesar_impacto(&temp_militar,bomba_actual);
        }
        temp_militar = temp_militar -> siguiente;
    }

    objetivo *temp_civil = *objetivos_civiles;
    while (temp_civil != NULL) {
        //printf("Objetivo civil alcanzado %i,(%i,%i)\n", esta_en_radio(temp_civil,bomba_actual),temp_civil -> coord1, temp_civil -> coord2);
        if (esta_en_radio(temp_civil, bomba_actual)) {
            procesar_impacto(&temp_civil,bomba_actual);
        }
        temp_civil = temp_civil -> siguiente;
    }
}

objetivo *clonar_objetivos(objetivo *objetivos) {
    objetivo *nuevos_objetivos = NULL;
    objetivo *temp = objetivos;
    while (temp != NULL) {
        agregar_objetivo(&nuevos_objetivos, temp -> coord1, temp -> coord2, temp -> resistencia);
        temp = temp -> siguiente;
    }
    return nuevos_objetivos;
}

void imprimir_respuesta(respuesta r) {
    printf("Respuesta:\nObjetivos intactos: %i\n", r.intactos);
    printf("Objetivos parcialmente destruidos: %i\n", r.parcial);
    printf("Objetivos totalmente destruidos: %i\n", r.destruidos);
}

respuesta comparar_objetivos(objetivo *estado_inicial, objetivo *estado_final) {
    objetivo *temp1 = estado_inicial;
    objetivo *temp2 = estado_final;
    respuesta r;
    r.intactos = 0;
    r.parcial = 0;
    r.destruidos = 0;
    while (temp1 != NULL) {
        if (temp2 -> resistencia == 0) {
            r.destruidos += 1;
        }
        else if (temp1 -> resistencia == temp2 -> resistencia) {
            r.intactos += 1;
        }
        else {
            r.parcial += 1;
        }
        temp1 = temp1 -> siguiente;
        temp2 = temp2 -> siguiente;
    }

    return r;
}