#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

enum STATE {
    THINKING,
    HUNGRY,
    EATING
};
enum STATE state[5];

/*mutex and condition for pthread_cond_wait*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond[5];

void test(int philosopher_number){
    if (state[(philosopher_number+4)%5] != EATING &&
        state[philosopher_number]       == HUNGRY &&
        state[(philosopher_number+1)%5] != EATING){
        state[philosopher_number] = EATING;
        pthread_mutex_lock(&mutex);
        pthread_cond_signal(&cond[philosopher_number]);
        pthread_mutex_unlock(&mutex);
    }
}
void pickup_forks(int philosopher_number){
    state[philosopher_number] = HUNGRY;
    printf("Philosopher %d is now HUNGRY and trying to pick up forks.\n", philosopher_number);
    test(philosopher_number);
    if (state[philosopher_number] != EATING) {
        printf("Philosopher %d can’t pick up forks and start waiting.\n", philosopher_number);
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond[philosopher_number], &mutex);
        pthread_mutex_unlock(&mutex);
    }
}
void return_forks(int philosopher_number){
    state[philosopher_number] = THINKING;
    int next = (philosopher_number+1)%5, prev = (philosopher_number+4)%5;
    printf("Philosopher %d returns forks and then starts TESTING %d and %d.\n", philosopher_number, prev, next);
    test(prev);
    test(next);
}

void philosophers(const int *n){

    //thinking
    state[*n] = THINKING;
    int time = rand()%3+1;
    printf("Philosopher %d is now THINKING for %d seconds.\n", *n, time);
    sleep(time);

    //become hungry
    pickup_forks(*n);

    //eating
    state[*n] = EATING;
    time = rand()%3+1;
    printf("Philosopher %d is now EATING.\n", *n);
    sleep(time);

    //end eating
    return_forks(*n);
}

int main(int argc, char *argv[]) {

    for (int i = 0; i < 5; ++i) {
        pthread_cond_init(&cond[i], NULL);
        state[i] = THINKING;
    }

    pthread_t id0, id1, id2, id3, id4;
    int n0 = 0, n1 = 1, n2 = 2, n3 = 3, n4 = 4;

    srand(time(NULL));

    //create the threads
    pthread_create(&id0, NULL, (void *)philosophers, &n0);
    pthread_create(&id1, NULL, (void *)philosophers, &n1);
    pthread_create(&id2, NULL, (void *)philosophers, &n2);
    pthread_create(&id3, NULL, (void *)philosophers, &n3);
    pthread_create(&id4, NULL, (void *)philosophers, &n4);

    //wait until all threads terminate
    pthread_join(id0, NULL);
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    pthread_join(id3, NULL);
    pthread_join(id4, NULL);

	pthread_mutex_destroy(&mutex);
    for (int i = 0; i < 5; ++i)
        pthread_cond_destroy(&cond[i]);

	return EXIT_SUCCESS;
}
