#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <fcntl.h>

typedef struct
{
    int nrThread;
    int nrPorcess;
} ThStrucut;

pthread_mutex_t mutex4;
pthread_mutex_t mutex3;

void verificareTh(ThStrucut *th)
{
    if (th->nrPorcess == 7 && th->nrThread == 3)
    {
        
        pthread_mutex_lock(&mutex3);
    }
    info(BEGIN, th->nrPorcess, th->nrThread);
    if (th->nrPorcess == 7 && th->nrThread == 4)
    {
        pthread_mutex_lock(&mutex4);
    }
    if (th->nrPorcess == 7 && th->nrThread == 4)
    {
        pthread_mutex_unlock(&mutex3);
    }
    info(END, th->nrPorcess, th->nrThread);

    if (th->nrPorcess == 7 && th->nrThread == 3)
    {
        pthread_mutex_unlock(&mutex4);
    }
}

int main()
{
    init();

    pid_t pid2, pid3, pid4, pid5, pid6, pid7, pid8, pid9;

    info(BEGIN, 1, 0);
    pid2 = fork();
    if (pid2 == 0)
    {
        info(BEGIN, 2, 0);

        pid3 = fork();

        if (pid3 == 0)
        {
            info(BEGIN, 3, 0);
            pid8 = fork();
            if (pid8 == 0)
            {
                info(BEGIN, 8, 0);
                info(END, 8, 0);
            }
            else
            {
                waitpid(pid8, NULL, 0);
                info(END, 3, 0);
            }
        }
        else
        {
            pid6 = fork();
            if (pid6 == 0)
            {
                info(BEGIN, 6, 0);
                info(END, 6, 0);
            }
            else
            {
                waitpid(pid3, NULL, 0);
                waitpid(pid6, NULL, 0);
                info(END, 2, 0);
            }
        }
    }
    else
    {
        pid4 = fork();
        if (pid4 == 0)
        {
            info(BEGIN, 4, 0);
            info(END, 4, 0);
        }
        else
        {
            pid5 = fork();
            if (pid5 == 0)
            {
                info(BEGIN, 5, 0);

                pid7 = fork();
                if (pid7 == 0)
                {
                    info(BEGIN, 7, 0);
                    pthread_mutex_init(&mutex4, NULL);
                    pthread_mutex_init(&mutex3, NULL);
                    pthread_t threads[4];

                    ThStrucut th[4] = {{1, 7}, {2, 7}, {3, 7}, {4, 7}};
                    pthread_mutex_lock (&mutex3);

                    for (int i = 0; i < 4; i++)
                        pthread_create(&threads[i], NULL, (void *)verificareTh, (void *)&th[i]);
                    for (int i = 0; i < 4; i++)
                        pthread_join(threads[i], NULL);

                    pid9 = fork();
                    if (pid9 == 0)
                    {
                        info(BEGIN, 9, 0);
                        info(END, 9, 0);
                    }
                    else
                    {
                        wait(NULL);
                        info(END, 7, 0);
                    }
                }
                else
                {
                    waitpid(pid7, NULL, 0);
                    info(END, 5, 0);
                }
            }
            else
            {
                waitpid(pid2, NULL, 0);
                waitpid(pid4, NULL, 0);
                waitpid(pid5, NULL, 0);
                info(END, 1, 0);
            }
        }
    }
    return 0;
}
