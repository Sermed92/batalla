/*
 * Archivo: main.c
 * Autores:
 *          RECUERDA COLOCARTE
 *          Sergio Medina 09-11259
 * Descripción:   
 */

/* Librerias */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

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
                nvalue = &optarg - '0';
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
 
   	int campoB[tamanio][tamanio];

    fscanf(archivo,"%d",&objetivos);

    // Se guardan los objetivos en la matriz
    int i;

    for (i = 0; i<objetivos; i++){
        fscanf(archivo,"%d %d %d", &cord1, &cord2, &valor);
        campoB[cord1][cord2] = valor;
    }

    fscanf(archivo,"%d",&numBombas);

    int bombas[numBombas][4];
    int radio, potencia;

    //Se guardan las bombas que serán usadas

    for (i = 0; i<numBombas; i++){
        fscanf(archivo,"%d %d %d %d", &cord1, &cord2, &radio, &potencia);
        bombas[i][0] = cord1;
        bombas[i][1] = cord2;
        bombas[i][2] = radio;
        bombas[i][3] = potencia;
    }

    fclose(archivo);

    if (hflag==1) {
        //Se trabajara con hilos
        printf("Soy hilos\n");
    } else {
        // p opcion por defecto, no entrara si se da el argumento de hilo
        //Se trabajara con procesos
        printf("Soy procesos\n");
    }

    return 0;

 }