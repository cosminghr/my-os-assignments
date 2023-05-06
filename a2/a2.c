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
    int nrProcess;
} ThStrucut;

sem_t *sem_t74_started;
sem_t *sem_t74_ended;
sem_t *sem2_1, *sem2_2;

void *thread_func_for2_3(ThStrucut *arg)
{
    if (arg->nrThread == 2 && arg->nrProcess == 7)
    {
        sem_wait(sem2_1);
        info(BEGIN, 7, 2);
        
        info(END, 7, 2);
        sem_post(sem2_2);
    }
    else if (arg->nrThread == 3)
    {
        sem_wait(sem_t74_started);
        info(BEGIN, 7, arg->nrThread);
        info(END, 7, arg->nrThread);
        sem_post(sem_t74_ended);
    }
    else if (arg->nrThread == 4)
    {
        info(BEGIN, 7, arg->nrThread);
        sem_post(sem_t74_started);
        sem_wait(sem_t74_ended);
        info(END, 7, arg->nrThread);
    }
    else
    {
        info(BEGIN, 7, arg->nrThread);
        info(END, 7, arg->nrThread);
    }

    return NULL;
}

void *thread_func_for2_4(ThStrucut *th)
{

    info(BEGIN, 4, th->nrThread);
    info(END, 4, th->nrThread);

    return NULL;
}
void *thread_func_for2_5(ThStrucut *th)
{

    if (th->nrProcess == 2 && th->nrThread == 2)
    {
        sem_wait(sem2_2);
        info(BEGIN, 2, 2);
        info(END, 2, 2);
    }
    else if (th->nrThread == 3 && th->nrProcess == 2)
    {
        info(BEGIN, 2, 3);
        info(END, 2, 3);
        sem_post(sem2_1);
    }
    else
    {
        info(BEGIN, 2, th->nrThread);
        info(END, 2, th->nrThread);
    }

    return NULL;
}

int main()
{
    init();

    unlink("sem2_1");
    unlink("sem2_2");
    unlink("sem_t74_started");
    unlink("sem_t74_ended");

    sem2_1 = sem_open("sem2_1", O_CREAT, 0644, 0);
    sem2_2 = sem_open("sem2_2", O_CREAT, 0644, 0);
    sem_t74_started = sem_open("sem_t74_started", O_CREAT, 0644, 0);
    sem_t74_ended = sem_open("sem_t74_ended", O_CREAT, 0644, 0);

    pid_t pid2, pid3, pid4, pid5, pid6, pid7, pid8, pid9;

    info(BEGIN, 1, 0);
    pid2 = fork();
    if (pid2 == 0)
    {
        info(BEGIN, 2, 0);

        pthread_t threads[5];
        ThStrucut th[5] = {{1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}};

        for (int i = 0; i < 5; i++)
        {
            pthread_create(&threads[i], NULL, (void *)thread_func_for2_5, &th[i]);
        }

        // wait for threads to finish
        for (int i = 0; i < 5; i++)
        {
            pthread_join(threads[i], NULL);
        }

        sem_destroy(sem2_1);
        sem_destroy(sem2_2);
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

            pthread_t threads[48];
            ThStrucut th[48];

            for (int i = 1; i <= 48; i++)
            {
                th[i].nrProcess = 4;
                th[i].nrThread = i;
                pthread_create(&threads[i - 1], NULL, (void *)thread_func_for2_4, &th[i]);
            }

            // wait for threads to finish
            for (int i = 0; i <= 47; i++)
            {
                pthread_join(threads[i], NULL);
            }

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

                    // create threads
                    pthread_t threads[4];
                    ThStrucut th[4] = {{1, 7}, {2, 7}, {3, 7}, {4, 7}};
                    for (int i = 0; i < 4; i++)
                    {
                        pthread_create(&threads[i], NULL, (void *)thread_func_for2_3, &th[i]);
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
