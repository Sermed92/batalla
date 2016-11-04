#ifndef batalla
#define batalla

/* Librerias */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/wait.h>
#include "batalla.h"

// Estructura para la cola de bombas
typedef struct _bomba {
        int coord1;
        int coord2;
        int potencia;
        int radio;
        struct _bomba *siguiente;
} bomba;

// Estructura para la respuesta de los procesos
typedef struct _respuesta {
        int intactosM;
        int parcialM;
        int destruidosM;
        int intactosC;
        int parcialC;
        int destruidosC;        
} respuesta;

// Estructura para la cola de objetivos
typedef struct _objetivo {
        int coord1;
        int coord2;
        int resistencia;
        struct _objetivo *siguiente;
} objetivo;

void imprimir_matriz(int, int);
void agregar_bomba(bomba**, int, int, int, int);
void imprimir_bombas(bomba*);

void imprimir_objetivos(objetivo*);
void agregar_objetivo(objetivo**, int, int, int);

void lanzar_lista_bombas(objetivo**, bomba*);
void lanzar_lista_bombas_proceso(objetivo **, bomba*, sem_t);

objetivo *clonar_objetivos(objetivo*);
void clonar_objetivos_compartida(objetivo*, objetivo**);

respuesta comparar_objetivos(objetivo*, objetivo*);
void imprimir_respuesta(respuesta);

void liberar_objetivos(objetivo**);
void liberar_bombas(bomba**);

#endif