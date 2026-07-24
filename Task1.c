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
pthread_mutex_t lock1, lock2;

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

void *deadlockThread1(void *arg)
{
    printf("Deadlock Thread 1: Locking Lock1...\n");
    pthread_mutex_lock(&lock1);

    sleep(1);

    printf("Deadlock Thread 1: Trying to lock Lock2...\n");

    if (pthread_mutex_trylock(&lock2) != 0)
    {
        printf("Deadlock Thread 1: Lock2 unavailable (Potential Deadlock Detected)\n");
    }
    else
    {
        pthread_mutex_unlock(&lock2);
    }

    pthread_mutex_unlock(&lock1);

    pthread_exit(NULL);
}

void *deadlockThread2(void *arg)
{
    printf("Deadlock Thread 2: Locking Lock2...\n");
    pthread_mutex_lock(&lock2);

    sleep(1);

    printf("Deadlock Thread 2: Trying to lock Lock1...\n");

    if (pthread_mutex_trylock(&lock1) != 0)
    {
        printf("Deadlock Thread 2: Lock1 unavailable (Potential Deadlock Detected)\n");
    }
    else
    {
        pthread_mutex_unlock(&lock1);
    }

    pthread_mutex_unlock(&lock2);

    pthread_exit(NULL);
}

void *safeThread1(void *arg)
{
    printf("Safe Thread 1: Locking Lock1...\n");

    pthread_mutex_lock(&lock1);

    sleep(1);

    printf("Safe Thread 1: Locking Lock2...\n");

    pthread_mutex_lock(&lock2);

    printf("Safe Thread 1 acquired both locks.\n");

    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);

    pthread_exit(NULL);
}

void *safeThread2(void *arg)
{
    printf("Safe Thread 2: Locking Lock1...\n");

    pthread_mutex_lock(&lock1);

    sleep(1);

    printf("Safe Thread 2: Locking Lock2...\n");

    pthread_mutex_lock(&lock2);

    printf("Safe Thread 2 acquired both locks.\n");

    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);

    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[THREADS];
    int id[THREADS];

    // Initialize Mutex
    pthread_mutex_init(&mutex,NULL);
    pthread_mutex_init(&lock1,NULL);
    pthread_mutex_init(&lock2,NULL);

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
    printf("\n==============================\n");
    printf("Deadlock Demonstration\n");
    printf("==============================\n");

    pthread_t d1,d2;

    pthread_create(&d1,NULL,deadlockThread1,NULL);
    pthread_create(&d2,NULL,deadlockThread2,NULL);

    pthread_join(d1,NULL);
    pthread_join(d2,NULL);

    printf("\nPotential deadlock demonstrated.\n");
    printf("Now preventing deadlock using consistent lock ordering.\n");

    printf("\n==============================\n");
    printf("Deadlock Prevention\n");
    printf("==============================\n");

    pthread_create(&d1,NULL,safeThread1,NULL);
    pthread_create(&d2,NULL,safeThread2,NULL);

    pthread_join(d1,NULL);
    pthread_join(d2,NULL);

    roundRobin();

    // Destroy synchronization objects
    pthread_mutex_destroy(&mutex);
    sem_destroy(&semaphore);
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);  

    return 0;
}
