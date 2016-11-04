/*
 * Archivo: main.c
 * Autores:
 *          Sergio Medina 09-11259
 *          Lucio Mederos 13-10856
 * Descripción:   
 */

#include "batalla.h"

// Lista de objetivos como global para poder ser leida por los hilos
objetivo *lista_objetivos;

void* llamada_a_lanzar_bomba(void *arg) {
    bomba* bomba_actual = (bomba*) arg;
    lanzar_lista_bombas(&lista_objetivos, bomba_actual);
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

    for (i = 0; i<objetivos; i++){
        fscanf(archivo,"%d %d %d", &cord1, &cord2, &valor);
        agregar_objetivo(&lista_objetivos, cord1,cord2,valor);        
    }

    objetivo *objetivos_originales = clonar_objetivos(lista_objetivos);

    fscanf(archivo,"%d",&numBombas);

    int radio, potencia;
    bomba *bombas = NULL;

    //Se guardan las bombas que serán usadas

    for (i = 0; i < numBombas; i++){
        fscanf(archivo,"%d %d %d %d", &cord1, &cord2, &radio, &potencia);
        agregar_bomba(&bombas,cord1, cord2, potencia, radio);
    }

    fclose(archivo);
    
    if (nvalue != 0 && nvalue >= numBombas){
        nvalue = numBombas;
    } else if (nvalue == 0) {
        nvalue = 1;
    }
    
    // Repartir bombas en un arreglo
    int cont = 0;
    bomba* arreglo_bombas [nvalue];
    while (cont < nvalue) {
        arreglo_bombas[cont] = NULL;
        cont++;
    }
    cont = 0;

    bomba *bomba_actual;
    while (bombas != NULL) {
        bomba_actual = bombas;
        bombas = bombas -> siguiente;
        bomba_actual -> siguiente = arreglo_bombas[cont%nvalue];
        arreglo_bombas[cont%nvalue] = bomba_actual;
        
        cont++;
    }
    /*/ Imprimir arreglo de bombas
    cont = 0;
    for (cont = 0; cont < nvalue; cont ++){
        printf("Bombas posicion: %d\n", cont);
        imprimir_bombas(arreglo_bombas[cont]);
    }
    /*/

    respuesta r;
    tiempo_inicio = clock();

    if (hflag==1) {
        //Se trabajara con hilos

        // Se inicializan los hilos
        pthread_t arreglo_hilos[nvalue];
        for (cont = 0; cont < nvalue; cont++) {
            pthread_create(&arreglo_hilos[cont], NULL, llamada_a_lanzar_bomba, arreglo_bombas[cont]);
        }
        // Se espera en el proceso principal a que terminen todos los hilos
        for (cont = 0; cont < nvalue; cont++) {
            pthread_join(arreglo_hilos[cont],NULL);
        }

        r = comparar_objetivos(objetivos_originales, lista_objetivos);

    } else {
        // p opcion por defecto, no entrara si se da el argumento de hilo
        //Se trabajara con procesos

        if (nvalue == 1) {
            // Solo trabajar sobre proceso padre
            lanzar_lista_bombas(&lista_objetivos, arreglo_bombas[0]);
        }
        else {
            pid_t pid_padre = getpid();
            pid_t arreglo_procesos[nvalue];
            int llave_semaforos = 15;
            int llave_objetivos = 16;
            int arreglo_semaforos_id = shmget(llave_semaforos, sizeof(sem_t), IPC_CREAT | 0775);
            int lista_objetivos_id = shmget(llave_objetivos, sizeof(objetivo*), IPC_CREAT | 0775);
            if (arreglo_semaforos_id == -1) {
                printf("Error en la alocacion de memoria para semaforo\n");
                 exit(1);
            }
            if (lista_objetivos_id == -1) {
                printf("Error en la alocacion de memoria para objetivos\n");
                exit(1);
            }
            // Se crea un semaforo en memoria compartida para evitar el problema de seccion critica
            sem_t* semaforo_seccion_critica = (sem_t*) shmat(arreglo_semaforos_id, NULL, 0);
            if (semaforo_seccion_critica == (void*)-1) {
                printf("Error al acoplar memoria compartida de semaforo\n");
                exit(1);
            }
            sem_init(&semaforo_seccion_critica[0], 1, 1);

            // Se crea un espacio en memoria compartida para compartir los bojetivos entre los procesos
            objetivo* lista_objetivos_compartida = (objetivo*) shmat(lista_objetivos_id, NULL, 0);
            clonar_objetivos_compartida(lista_objetivos, &lista_objetivos_compartida);
            
            for (cont = 0; cont < nvalue; cont++) {
                if (getpid() == pid_padre) {
                    arreglo_procesos[cont] = fork();
                }
                else {
                    break;
                }
            }

            /// Poner a trabajar a los hijos
            if (getpid() != pid_padre) {
                cont--;
                lanzar_lista_bombas_proceso(&lista_objetivos_compartida, arreglo_bombas[cont], semaforo_seccion_critica[0]);
                exit(1);
            } else {
                // Se espera en el proceso principal a que terminen todos los hijos
                for (cont = 0; cont < nvalue; cont++) {
                    wait(&arreglo_procesos[cont]);\
                }
            }

            r = comparar_objetivos(objetivos_originales, lista_objetivos_compartida);

        }

    }

    imprimir_respuesta(r);

    // Se toma el tiempo final y se calcula el tiempo usado
    tiempo_final = clock();
    tiempo_usado = ((double) tiempo_final - tiempo_inicio) / CLOCKS_PER_SEC;
    printf("Tiempo usado: %f\n", tiempo_usado);

    return 0;

}