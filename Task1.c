#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define THREADS 3
#define TIME_QUANTUM 2

// Shared resource
int counter = 0;

// Mutex
pthread_mutex_t mutex;

// Semaphore
sem_t semaphore;

// Round Robin Burst Times
int burstTime[THREADS] = {5, 7, 4};

void roundRobin()
{
    int remaining[THREADS];

    for(int i=0;i<THREADS;i++)
        remaining[i]=burstTime[i];

    printf("\n===== Round Robin Scheduling =====\n");

    int done;

    while(1)
    {
        done=1;

        for(int i=0;i<THREADS;i++)
        {
            if(remaining[i]>0)
            {
                done=0;

                printf("Thread %d executing...\n",i+1);

                if(remaining[i]>TIME_QUANTUM)
                {
                    sleep(1);
                    remaining[i]-=TIME_QUANTUM;
                }
                else
                {
                    sleep(1);
                    remaining[i]=0;
                    printf("Thread %d Finished\n",i+1);
                }
            }
        }

        if(done)
            break;
    }
}

void *worker(void *arg)
{
    int id=*(int*)arg;

    // Wait on semaphore
    sem_wait(&semaphore);

    printf("Thread %d entered critical section\n",id);

    // Lock mutex
    pthread_mutex_lock(&mutex);

    for(int i=0;i<5;i++)
    {
        counter++;

        printf("Thread %d Counter=%d\n",id,counter);

        sleep(1);
    }

    pthread_mutex_unlock(&mutex);

    printf("Thread %d leaving critical section\n",id);

    sem_post(&semaphore);

    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[THREADS];
    int id[THREADS];

    // Initialize Mutex
    pthread_mutex_init(&mutex,NULL);

    // Allow only 2 threads simultaneously
    sem_init(&semaphore,0,2);

    printf("Process Started\n");

    // Create Threads
    for(int i=0;i<THREADS;i++)
    {
        id[i]=i+1;
        pthread_create(&threads[i],NULL,worker,&id[i]);
    }

    // Wait for Threads
    for(int i=0;i<THREADS;i++)
    {
        pthread_join(threads[i],NULL);
    }

    printf("\nFinal Counter = %d\n",counter);

    roundRobin();

    // Destroy synchronization objects
    pthread_mutex_destroy(&mutex);
    sem_destroy(&semaphore);

    return 0;
}