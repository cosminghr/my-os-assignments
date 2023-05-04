#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

typedef struct
{
    int nrThread;
    int nrPorcess;
} ThStrucut;

sem_t sem;
void *thread_func(ThStrucut *arg)
{

    info(BEGIN, 7, arg->nrThread);

    if (arg->nrThread == 4)
    {
        sem_wait(&sem); // wait for T7.3 to start
    }


    if (arg->nrThread == 3)
    {
        sem_post(&sem); // signal T7.4 to start
    }

    info(END, 7, arg->nrThread);

    return NULL;
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

                    sem_init(&sem, 0, 0);

                    // create threads
                    pthread_t threads[4];
                    ThStrucut th[4] = {{1, 7}, {2, 7}, {3, 7}, {4, 7}};
                    for (int i = 0; i < 4; i++)
                    {
                        pthread_create(&threads[i], NULL, (void*) thread_func, &th[i]);
                    }

                    // wait for threads to finish
                    for (int i = 0; i < 4; i++)
                    {
                        pthread_join(threads[i], NULL);
                    }
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
