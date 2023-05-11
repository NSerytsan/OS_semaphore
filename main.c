#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUMBER_OF_PHILOSOPHERS 5

enum state
{
    THINKING,
    HUNGRY,
    EATING
};

int left_fork(int idx)
{
    return (idx + 4) % NUMBER_OF_PHILOSOPHERS;
}

int right_fork(int idx)
{
    return (idx + 1) % NUMBER_OF_PHILOSOPHERS;
}

int state[NUMBER_OF_PHILOSOPHERS];
int philosophers[NUMBER_OF_PHILOSOPHERS] = {0, 1, 2, 3, 4};

sem_t mutex;
sem_t signals[NUMBER_OF_PHILOSOPHERS];

void check(int philosopher)
{
    if (state[philosopher] == HUNGRY && state[left_fork(philosopher)] != EATING && state[right_fork(philosopher)] != EATING)
    {
        state[philosopher] = EATING;

        sleep(2);

        printf("Philosopher %d takes fork %d and %d\n",
               philosopher + 1, left_fork(philosopher) + 1, philosopher + 1);

        printf("Philosopher %d is Eating\n", philosopher + 1);

        sem_post(&signals[philosopher]);
    }
}

void take_fork(int philosopher)
{

    sem_wait(&mutex);

    state[philosopher] = HUNGRY;

    printf("Philosopher %d is Hungry\n", philosopher + 1);

    check(philosopher);

    sem_post(&mutex);

    sem_wait(&signals[philosopher]);

    sleep(1);
}

void put_fork(int philosopher)
{
    sem_wait(&mutex);

    state[philosopher] = THINKING;

    printf("Philosopher %d putting fork %d and %d down\n",
           philosopher + 1, left_fork(philosopher) + 1, philosopher + 1);
    printf("Philosopher %d is Thinking\n", philosopher + 1);

    check(left_fork(philosopher));
    check(right_fork(philosopher));

    sem_post(&mutex);
}

void *philosopher(void *id)
{
    int philosopher_id = *(int *)id;

    while (1)
    {
        sleep(1);

        take_fork(philosopher_id);

        sleep(0);

        put_fork(philosopher_id);
    }
}

int main()
{
    pthread_t thread_id[NUMBER_OF_PHILOSOPHERS];

    sem_init(&mutex, 0, 1);

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++)

        sem_init(&signals[i], 0, 0);

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++)
    {
        pthread_create(&thread_id[i], NULL,
                       philosopher, &philosophers[i]);

        printf("Philosopher %d is Thinking\n", i + 1);
    }

    for (int i = 0; i < NUMBER_OF_PHILOSOPHERS; i++)
    {
        pthread_join(thread_id[i], NULL);
    }
}