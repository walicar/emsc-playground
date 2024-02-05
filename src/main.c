#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h>

#define NUMTHRDS 4
#define MAGNIFICATION 1e9

typedef struct
{
    int thread_id;
    double *sum;
} Arg;

pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexsum;

void *count_pi(void *arg)
{

    Arg *data = (Arg *)arg;
    int thread_id = data->thread_id;
    double *sum = data->sum;
    pthread_mutex_lock(&mutexsum);
    *sum += 1;
    pthread_mutex_unlock(&mutexsum);

    EM_ASM_INT({
        console.log('Thread ' + $0);
    }, thread_id);
    printf("Thread %d: sum=%f\n", thread_id, *sum);

    pthread_exit((void *)0);
}

int main(int argc, char *argv[])
{
    pthread_mutex_init(&mutexsum, NULL);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    double *sum = malloc(sizeof(*sum));
    *sum = 0;

    Arg arg[NUMTHRDS];
    for (int i = 0; i < NUMTHRDS; i++)
    {
        arg[i].thread_id = i;
        arg[i].sum = sum;
        pthread_create(&callThd[i], &attr, count_pi, (void *)&arg[i]);
    }

    pthread_attr_destroy(&attr);

    void *status;
    for (int i = 0; i < NUMTHRDS; i++)
    {
        pthread_join(callThd[i], &status);
    }

    printf("Final Sum =  %f \n", *sum);
    free(sum);

    EM_ASM({
        console.log('Hello from C!');
    });

    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);

    return 0;
}