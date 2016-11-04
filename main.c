/*
 * Archivo: main.c
 * Autores:
 *          Sergio Medina 09-11259
 *          Lucio Mederos 13-10856
 * Descripción:   
 */

#include "batalla.h"

// Lista de objetivos global
objetivo *lista_objetivos;

void* llamada_a_lanzar_bomba(void *arg) {
    bomba* bomba_actual = (bomba*) arg;
    lanzar_lista_bombas(&lista_objetivos, bomba_actual);
    return NULL;
}

// Finaliza el programa si no posee la cantidad de argumentos necesarios
// Indica la forma correcta
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

    // Variables para la lectura de argumentos
    int hflag = 0;
    int pflag = 0;
    int nvalue = 0;
    int n;

    // Variables para medir el tiempo en que se lanzan las bombas
    clock_t tiempo_inicio, tiempo_final;
    double tiempo_usado;

    opterr = 0;

    // Se procesan los argumentos
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

    // Abortar si se intenta trabajar con hilos y procesos
    if ((hflag==1) && (pflag==1)){
        printf("-h y -p son opciones excluyentes\n");
        exit(1);
    }

    // Seccion de lectura del archivo
    FILE *archivo = NULL;
    archivo = fopen(argv[optind], "r");

    // No se encuentra en el directorio
    if (archivo == NULL) {
        printf(" ERROR: Archivo no encontrado \n");
        exit(1);
    }

    // Variables para la lectura de los valores en el archivo
    int tamanio, objetivos, numBombas, cord1, cord2, valor, radio, potencia;
    int cont; // contador multipropositos
    lista_objetivos = NULL; // Se prepara la lista para trabajar en ella

    fscanf(archivo,"%d",&tamanio);
    fscanf(archivo,"%d",&objetivos);

    // Se almacenan los objetivos de importancia en el mapa
    for (cont = 0; cont<objetivos; cont++){
        fscanf(archivo,"%d %d %d", &cord1, &cord2, &valor);

        // Las coordenadas deben existir en el mapa
        if (cord1 > 0 && cord1 <tamanio){
            if (cord2 > 0 && cord2 < tamanio){
                agregar_objetivo(&lista_objetivos, cord1,cord2,valor);                        
            }
        }
    }

    // Se conserva el estado original del Campo de batalla
    objetivo *objetivos_originales = clonar_objetivos(lista_objetivos);


    fscanf(archivo,"%d",&numBombas);
    bomba *bombas = NULL;   // Lista para almacenar las bombas

    //Se guardan las bombas que seran usadas
    for (cont = 0; cont < numBombas; cont++){
        fscanf(archivo,"%d %d %d %d", &cord1, &cord2, &radio, &potencia);

        // Las coordenadas deben existir en el mapa
        if (cord1 > 0 && cord1 <tamanio){
            if (cord2 > 0 && cord2 < tamanio){        
                agregar_bomba(&bombas,cord1, cord2, potencia, radio);
            }
        }
    }

    fclose(archivo);    // Archivo leido, se cierra 
    
    // Si la cantidad de procesos/hilos es mayor que el numero de bombas
    // Se le asigna la cantidad de bombas (y se repartira 1 bomba por proceso)
    if (nvalue != 0 && nvalue >= numBombas){
        nvalue = numBombas;
    } else if (nvalue == 0) {
        // Si no se usa el argumento -n, se le asigna 1 para crear el arreglo
        nvalue = 1;
    }
    
    // Repartir bombas en un arreglo de tamanio nvalue
    cont = 0;
    bomba* arreglo_bombas [nvalue];
    // Inicializar arreglo (evitar que haya basura)
    while (cont < nvalue) {
        arreglo_bombas[cont] = NULL;
        cont++;
    }

    cont = 0;
    bomba *bomba_actual;
    // Se agregan las bombas al arreglo
    while (bombas != NULL) {
        bomba_actual = bombas;
        bombas = bombas -> siguiente;
        // Se usa mod(cont, nvalue) para recorrer el arreglo varias veces
        bomba_actual -> siguiente = arreglo_bombas[cont%nvalue];
        arreglo_bombas[cont%nvalue] = bomba_actual;
        
        cont++;
    }

    respuesta r;             // Se prepara la respuesta
    tiempo_inicio = clock(); // Se guarda el tiempo inicial

    if (hflag==1) {
        //Se trabajara con hilos

        // Se crea el arreglo de hilos
        pthread_t arreglo_hilos[nvalue];

        // Se crean los hilos y se les asigna la tarea a realizar y el espacio sobre el cual hacerlo
        for (cont = 0; cont < nvalue; cont++) {
            pthread_create(&arreglo_hilos[cont], NULL, llamada_a_lanzar_bomba, arreglo_bombas[cont]);
        }
        // Se espera en el proceso principal a que terminen todos los hilos
        for (cont = 0; cont < nvalue; cont++) {
            pthread_join(arreglo_hilos[cont],NULL);
        }

        // Se guarda en la respuesta las diferencias entre la lista original y la modificada por los hilos
        r = comparar_objetivos(objetivos_originales, lista_objetivos);

    } else {
        // Procesos es la opcion por defecto

        if (nvalue == 1) {
            // Solo trabajar sobre proceso padre
            lanzar_lista_bombas(&lista_objetivos, arreglo_bombas[0]);
        }
        else {

            // COMENTA AQUI PLS
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

    // Se imprime la respuesta
    imprimir_respuesta(r);

    // Se toma el tiempo fina, se calcula el tiempo usado, se imprime para comparar resultados
    tiempo_final = clock();
    tiempo_usado = ((double) tiempo_final - tiempo_inicio) / CLOCKS_PER_SEC;
    printf("Tiempo usado: %f\n", tiempo_usado);

    return 0;

}