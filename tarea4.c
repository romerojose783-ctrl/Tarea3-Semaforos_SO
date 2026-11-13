#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_NUM 10000

// 1. Declaración del semáforo para la sección crítica
sem_t sem_bloqueo;

// Estructura para pasar el identificador a cada hilo
typedef struct {
    int id_hilo;
} datos_hilo;

// Función que ejecutan los hilos
void* imprimir_numeracion(void* arg) {
    datos_hilo *datos = (datos_hilo*) arg;
    int id = datos->id_hilo;

    for (int i = 1; i <= MAX_NUM; i++) {
        
        // REQUISITO: Al llegar exactamente a 4000, adquirimos el semáforo.
        // Si el otro hilo ya está aquí, este hilo se esperará sin interrumpir.
        if (i == 4000) {
            sem_wait(&sem_bloqueo); 
        }

        // Impresión en consola
        if (id == 1) {
            printf("El hilo 1 dice... %d\n", i);
        } else {
            printf("El hilo 2 dice... %d\n", i);
        }
        fflush(stdout); // Forzar salida inmediata en pantalla

        // REQUISITO: Al terminar de imprimir el 6000, liberamos el semáforo
        // para que el otro hilo pueda entrar a su sección crítica si lo desea.
        if (i == 6000) {
            sem_post(&sem_bloqueo); 
        }
    }

    free(datos);
    return NULL;
}

int main() {
    pthread_t hilo1, hilo2;

    // 2. Inicializar el semáforo con un valor de 1 (exclusión mutua)
    // El segundo argumento '0' indica que es compartido solo entre hilos de este proceso.
    if (sem_init(&sem_bloqueo, 0, 1) != 0) {
        perror("Error al inicializar el semáforo");
        exit(1);
    }

    // Reservar memoria y asignar IDs para los hilos
    datos_hilo *arg1 = malloc(sizeof(datos_hilo));
    arg1->id_hilo = 1;

    datos_hilo *arg2 = malloc(sizeof(datos_hilo));
    arg2->id_hilo = 2;

    // Crear los hilos
    pthread_create(&hilo1, NULL, imprimir_numeracion, (void*) arg1);
    pthread_create(&hilo2, NULL, imprimir_numeracion, (void*) arg2);

    // Esperar a que ambos hilos terminen
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

//### 3. Destruir el semáforo y liberar recursos al finalizar
    sem_destroy(&sem_bloqueo);

    printf("¡Proceso completado exitosamente con semáforos!\n");

    return 0;
}
