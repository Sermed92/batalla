#ifdef batalla
#define batalla

// Estructura para la cola de bombas
struct bomba {
        int coord1;
        int coord2;
        int potencia;
        int radio;
        struct bomba *siguiente;
};

// Estructura para la respuesta de los procesos
typedef struct _respuesta {
        int om_intactos;
        int om_parcial;
        int om_destruidos;
        int oc_ictactos;
        int oc_parcial;
        int oc_destruidos;
} respuesta;

void imprimir_matriz(int, int);


#endif