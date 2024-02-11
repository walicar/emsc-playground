#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdbool.h>
#include <emscripten.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMTHRDS 4
#define MAGNIFICATION 1e9

SDL_Surface *screen = NULL;
SDL_Surface *image = NULL;

typedef struct
{
    int thread_id;
    double *sum;
} Arg;

pthread_t callThd[NUMTHRDS];
pthread_mutex_t mutexsum;

static bool is_running = false;

bool init_sdl()
{
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL init ERR: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        screen = SDL_SetVideoMode(256, 256, 32, SDL_SWSURFACE);
        if (SDL_MUSTLOCK(screen))
            SDL_LockSurface(screen);
        if (screen == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        if (SDL_MUSTLOCK(screen))
            SDL_UnlockSurface(screen);

        SDL_Flip(screen); // swap screen buffer
    }
    return success;
}

bool load_media()
{
    bool success = true;
    int flags = IMG_INIT_PNG;
    if (!(IMG_Init(flags) & flags))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        success = false;
    }

    image = IMG_Load("src/smile.png");
    if (image == NULL)
    {
        printf("Unable to load image %s! SDL Error: %s\n", "smile.png", IMG_GetError());
        success = false;
    }
    return success;
}

void close_sdl()
{
    SDL_FreeSurface(image);
    SDL_FreeSurface(screen);
    SDL_Quit();
}

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

static void mainloop(void)
{
    SDL_Event e;
    if (!is_running)
    {
        // clean up
        emscripten_cancel_main_loop();
        close_sdl();
    }

    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            is_running = false;
        }
    }
    // run it
}

int main(int argc, char *argv[])
{
    // pthread stuff
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

    // SDL stuff

    if (!init_sdl())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        // Load media
        if (!load_media())
        {
            printf("Failed to load media!\n");
        }
        else
        {
            // Apply the image
            SDL_BlitSurface(image, NULL, screen, NULL);
        }
    }
    close_sdl();
    // cleanup
    pthread_exit(NULL);

    return 0;
}