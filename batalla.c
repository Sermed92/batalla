#include "batalla.h"

int llave = 5;

// Funcion para crear un nuevo nodo de tipo bomba
bomba *nueva_bomba(int coord1, int coord2, int potencia, int radio) {
    bomba *bomba_nueva = (bomba*) malloc(sizeof(bomba));
     
    if (bomba_nueva == NULL){
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

// Procedimiento para agregar un nuevo nodo tipo bomba a una lista de bombas
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

// Procedimiento para imprimir los nodos de una lista de bombas
void imprimir_bombas(bomba *bombas) {
    bomba *aux = bombas;
    while (aux != NULL)
    {
        printf("bomba: %i,%i,p:%i,r:%i\n", aux ->coord1,aux->coord2,aux->potencia,aux->radio);
        aux = aux->siguiente;
    }
}

// Funcion para crear un nuevo nodo de tipo objetivo
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

// Procedimiento para agregar un nuevo nodo tipo objetivo a una lista de objetivos
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

// Procedimiento para imprimir los nodos de una lista de objetivos
void imprimir_objetivos(objetivo *objetivos) {
    objetivo *aux = objetivos;
    while (aux != NULL)
    {
        printf("objetivo: %i,%i,%i\n", aux ->coord1,aux->coord2,aux->resistencia);
        aux = aux->siguiente;
    }
}

// Procedimiento para tomar el primer nodo de una lista de tipo bomba
bomba *pop_bomba (bomba** lista_bombas) {
    if (*lista_bombas == NULL) {
        return NULL;
    }
    else {
        bomba *primera_bomba = *lista_bombas;
        *lista_bombas = (*lista_bombas) -> siguiente;
        return primera_bomba;
    }
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

// Procedimiento para procesar un impacto sobre un objetivos
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

// Procedimiento para simular un ataque con bomba sobre los objetivos
void lanzar_bomba(objetivo **objetivos, bomba *bomba_actual) {
    objetivo *objetivo_actual = *objetivos;
    while (objetivo_actual != NULL) {
        if (esta_en_radio(objetivo_actual, bomba_actual)) {
            procesar_impacto(&objetivo_actual,bomba_actual);
        }
        objetivo_actual = objetivo_actual -> siguiente;
    }
}

// Funcion para lanzar una lista de bombas
void lanzar_lista_bombas(objetivo **objetivos, bomba *bombas_actuales) {
    bomba *bomba_actual = bombas_actuales;
    while (bomba_actual != NULL) {
        lanzar_bomba(objetivos, bomba_actual);
        bomba_actual = bomba_actual -> siguiente;
    }
}

void lanzar_bomba_proceso(objetivo **objetivos, bomba *bomba_actual, sem_t semaforo) {
    objetivo *objetivo_actual = *objetivos;
    while (objetivo_actual != NULL) {
        if (esta_en_radio(objetivo_actual, bomba_actual)) {
            sem_wait(&semaforo);
            procesar_impacto(&objetivo_actual, bomba_actual);
            sem_post(&semaforo);
        }
        objetivo_actual = objetivo_actual -> siguiente;
    }
}

void lanzar_lista_bombas_proceso(objetivo **objetivos, bomba *bombas_actuales, sem_t semaforo) {
    bomba *bomba_actual = bombas_actuales;
    while (bomba_actual != NULL) {
        lanzar_bomba_proceso(objetivos, bomba_actual, semaforo);
        bomba_actual = bomba_actual -> siguiente;
    }
}

// Funcion para crear una copiar de una lista de objetivos
objetivo *clonar_objetivos(objetivo *objetivos) {
    if (objetivos == NULL) {
        return NULL;
    }
    objetivo *temp = objetivos;
    objetivo *nuevos_objetivos = nuevo_objetivo(temp-> coord1, temp -> coord2, temp -> resistencia);
    objetivo *tempN = nuevos_objetivos;
    temp = temp -> siguiente;
    while (temp != NULL) {
        tempN -> siguiente = nuevo_objetivo(temp-> coord1, temp -> coord2, temp -> resistencia);
        temp = temp -> siguiente;
        tempN = tempN -> siguiente;
    }
    return nuevos_objetivos;
}

// Funcion para clonar un objetivo en memoria compartida devolviendo su apuntador
objetivo *nuevo_objetivo_compartida(objetivo *objetivo_actual) {
    llave++;
    int objetivo_id = shmget(llave, sizeof(objetivo), IPC_CREAT | 0775);
    if (objetivo_id == -1) {
        printf("Error en la alocacion de memoria compartida para objetivo");
        exit(1);
    }

    objetivo *nuevo_objetivo = (objetivo*) shmat(objetivo_id, NULL, 0);
    if (nuevo_objetivo == (void*)-1) {
        printf("Error en la acoplacion de memoria compartida para un objetivo\n");
        exit(1);
    }
    nuevo_objetivo -> coord1 = objetivo_actual -> coord1;
    nuevo_objetivo -> coord2 = objetivo_actual -> coord2;
    nuevo_objetivo -> resistencia = objetivo_actual -> resistencia;
    nuevo_objetivo -> siguiente = NULL;
    return nuevo_objetivo;
}

// Procedimiento para clonar una lista de objetivos en memoria compartida
// recibiendo el primer apuntador
void clonar_objetivos_compartida(objetivo *objetivos, objetivo **objetivos_compartidos) {
    if (objetivos == NULL) {
        return;
    }
    objetivo *objetivo_actual = objetivos;
    *objetivos_compartidos = nuevo_objetivo_compartida(objetivo_actual);
    objetivo *temp = objetivos -> siguiente;
    objetivo *temp_nuevo = *objetivos_compartidos;
    while (temp != NULL) {
        temp_nuevo -> siguiente = nuevo_objetivo_compartida(temp);
        temp = temp ->siguiente;
        temp_nuevo = temp_nuevo -> siguiente;
    }
}

// Procedimiento para mostrar una respuesta
void imprimir_respuesta(respuesta r) {
    printf("Objetivos militares intactos: %d\n", r.intactosM);
    printf("Objetivos militares parcialmente destruidos: %d\n", r.parcialM);
    printf("Objetivos militares totalmente destruidos: %d\n", r.destruidosM);
    printf("Objetivos civiles intactos: %d\n", r.intactosC);
    printf("Objetivos civiles parcialmente destruidos: %d\n", r.parcialC);
    printf("Objetivos civiles totalmente destruidos: %d\n", r.destruidosC);
}

// Funcion para comparar los objetivos con su estado inicial y dar una respuesta
respuesta comparar_objetivos(objetivo *estado_inicial, objetivo *estado_final) {
    objetivo *tempI = estado_inicial;
    objetivo *tempF = estado_final;
    respuesta r;
    r.intactosM = 0;
    r.parcialM = 0;
    r.destruidosM = 0;
    r.intactosC = 0;
    r.parcialC = 0;
    r.destruidosC = 0;    

    while (tempI != NULL) {
        if (tempI -> resistencia < 0) {
            if (tempF -> resistencia == 0) {
                r.destruidosM += 1;
            }
            else if (tempI -> resistencia == tempF -> resistencia) {
                r.intactosM += 1;
            }
            else {
                r.parcialM += 1;
            }
        }
        else if (tempI -> resistencia > 0) {
            if (tempF -> resistencia == 0) {
                r.destruidosC += 1;
            }
            else if (tempI -> resistencia == tempF -> resistencia) {
                r.intactosC += 1;
            }
            else {
                r.parcialC += 1;
            }
        }
        tempI = tempI -> siguiente;
        tempF = tempF -> siguiente;
    }

    return r;
}

// Procedimiento para liberar la memoria usada por una lista de objetivos
void liberar_objetivos(objetivo **objetivos) {
    objetivo *objetivo_actual = *objetivos;
    while(objetivo_actual != NULL) {
        *objetivos = (*objetivos) -> siguiente;
        if (objetivo_actual != NULL) {
            free(objetivo_actual);
        }
        objetivo_actual = *objetivos;
    }
}

// Procedimiento para liberar la memoria usada por una lista de bombas
void liberar_bombas(bomba **bombas) {
    bomba *bomba_actual = *bombas;
    while(bomba_actual != NULL) {
        *bombas = (*bombas) -> siguiente;
        if (bomba_actual != NULL) {
            free(bomba_actual);
        }
        bomba_actual = *bombas;
    }
}