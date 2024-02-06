#include <SDL/SDL.h>
#include <emscripten.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

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

    EM_ASM_INT({ console.log('Thread ' + $0); }, thread_id);
    printf("Thread %d: sum=%f\n", thread_id, *sum);

    pthread_exit((void *)0);
}

int main(int argc, char *argv[])
{
    printf("program started!\n");
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

    EM_ASM({ console.log('Hello from C!'); });

    pthread_mutex_destroy(&mutexsum);

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface *screen = SDL_SetVideoMode(256, 256, 32, SDL_SWSURFACE);

    if (SDL_MUSTLOCK(screen))
        SDL_LockSurface(screen);
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            // To emulate native behavior with blitting to screen, alpha component is
            // ignored. Test that it is so by outputting data (and testing that it
            // does get discarded)
            int alpha = (i + j) % 255;
            *((Uint32 *)screen->pixels + i * 256 + j) =
                SDL_MapRGBA(screen->format, i, j, 255 - i, alpha);
        }
    }

    if (SDL_MUSTLOCK(screen))
        SDL_UnlockSurface(screen);
    SDL_Flip(screen);

    printf("you should see a smoothly-colored square - no sharp lines but the "
           "square borders!\n");

    SDL_Quit();
    pthread_exit(NULL);

    return 0;
}