/*
 * Archivo: main.c
 * Autores:
 *          Sergio Medina 09-11259
 *          Lucio Mederos 13-10856
 * Descripción:   
 */

/* Librerias */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include "batalla.h"

// Lista de objetivos como global para poder ser leida por los hilos
objetivo *lista_objetivos;

void* llamada_a_lanzar_bomba(void *arg) {
    bomba* bomba_actual = (bomba*) arg;
    lanzar_bomba(&lista_objetivos, bomba_actual);
    return NULL;
}

// Finaliza el programa si no posee la cantidad de argumentos necesarios, despues de indicar la forma correcta
 void numeroArgumentos(int cantidad){
    if ((cantidad < 2) || (cantidad > 5)){
        printf("Violacion de parametros, pruebe: \n");
        printf("./batalla [-p] [-h] [-n <cantidad>] <archivo_entrada> \n");
        printf("Notese que los parametros entre [] son opcionales\n");
        exit(1);
    }
 }

 int main (int argc, char** argv){

    numeroArgumentos(argc);

    int hflag = 0;
    int pflag = 0;
    int nvalue = 0;
    int n;

    clock_t tiempo_inicio, tiempo_final;
    double tiempo_usado;

    /*
        HAY QUE DECICIR SI SE TOMARA EL TIEMPO DE LECTURA DEL
        ARCHIVO O NO. SI SE TOMA EN CUANTA COLOCAR AQUI:
        tiempo_inicio = clock();
    */

    opterr = 0;

    while ((n = getopt(argc, argv, "hpn:")) != -1){
        switch(n){
            case 'h':
            {
                hflag = 1;
                break;
            }
            case 'p':
            {
                pflag = 1;
                break;
            }
            case 'n':
            {
                nvalue = atoi(optarg);
                break;
            }
            case '?':
            {
                if (optopt == 'n'){
                    fprintf(stderr, "Option -%c requiere un argumento. \n",optopt);
                } else if (isprint(optopt)){
                    fprintf(stderr, "Opcion desconocida '-%c'.\n", optopt);
                } else{
                    fprintf(stderr, "Caracter desconocido '\\x%x'.\n", optopt);
                }
                return 1;
            }
            default:
            {
                abort();
            }
        }
    }

    if ((hflag==1) && (pflag==1)){
        printf("-h y -p son opciones excluyentes\n");
        exit(1);
    }

    // Seccion de lectura del archivo
    FILE *archivo = NULL;
    archivo = fopen(argv[optind], "r");

    if (archivo == NULL) {
        printf(" ERROR: Archivo no encontrado \n");
        exit(1);
    }

    int tamanio, objetivos, numBombas, cord1, cord2, valor;
    
    fscanf(archivo,"%d",&tamanio);

    int i;
    
    lista_objetivos = NULL;

    fscanf(archivo,"%d",&objetivos);

    // Se guardan los objetivos en la matriz
    

    for (i = 0; i<objetivos; i++){
        fscanf(archivo,"%d %d %d", &cord1, &cord2, &valor);
        agregar_objetivo(&lista_objetivos, cord1,cord2,valor);        
    }
    printf("Estado inicial:\n");
    imprimir_objetivos(lista_objetivos);

    objetivo *objetivos_originales = clonar_objetivos(lista_objetivos);

    fscanf(archivo,"%d",&numBombas);

    int radio, potencia;
    bomba *bombas = NULL;

    //Se guardan las bombas que serán usadas

    for (i = 0; i<numBombas; i++){
        fscanf(archivo,"%d %d %d %d", &cord1, &cord2, &radio, &potencia);
        agregar_bomba(&bombas,cord1, cord2, potencia, radio);
    }

    fclose(archivo);

    tiempo_inicio = clock();

    bomba *temp = bombas;
    /**
    int bombas_por_hijo;

    if (nvalue != 0 && nvalue >= numBombas){
        nvalue = numBombas;

    } else if (nvalue != 0){
        bombas_por_hijo = numBombas / nvalue;
    }
    printf("Numero de bombas que debe tener cada hijo: %i\n", bombas_por_hijo );
    */
    if (hflag==1) {
        //Se trabajara con hilos
        printf("Soy hilos");
        printf("N=%i\n",nvalue);

        pthread_t hilo;
        while (temp != NULL) {
            pthread_create(&hilo, NULL, llamada_a_lanzar_bomba, temp);
            pthread_join(hilo,NULL);
            temp = temp -> siguiente;
        }        
    } else {
        // p opcion por defecto, no entrara si se da el argumento de hilo
        //Se trabajara con procesos
        printf("Soy procesos");
        printf("N=%i\n",nvalue);

        while (temp != NULL) {
            llamada_a_lanzar_bomba(temp);
            temp = temp -> siguiente;
        }        
    }

    printf("Estado final:\n");
    imprimir_objetivos(lista_objetivos);

    respuesta r = comparar_objetivos(objetivos_originales, lista_objetivos);

    imprimir_respuesta(r);

    // Se toma el tiempo final y se calcula el tiempo usado
    tiempo_final = clock();
    tiempo_usado = ((double) tiempo_final - tiempo_inicio) / CLOCKS_PER_SEC;
    printf("Tiempo usado: %f\n", tiempo_usado);

    return 0;

}