#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <climits>
#include <random>
#include <iomanip>

#include <pthread.h>

struct ThreadArgs {
    int id;
    long long int iterations;
    long long int result;
};

float get_rand_between(long long int a, long long int b)
{
    thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> distribution(a, b);
    return distribution(generator);
}

void *toss(void *args)
{
    struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
    float x, y, squared_dist;
    for (int i = 0; i < thread_args->iterations; i++) {
        x = get_rand_between(0, 1);
        y = get_rand_between(0, 1);
        squared_dist = x * x + y * y;
        if (squared_dist <= 1) {
            thread_args->result += 1;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " [Thread number] [Tosses]" << std::endl;
        return 1;
    }

    int nthreads = std::atoi(argv[1]);
    long long int number_in_circle = 0;
    long long int total_tosses = std::atoll(argv[2]);

    if (nthreads <= 0) {
        std::cerr << "Thread number should be a positive integer" << std::endl;
        return 1;
    }

    if (total_tosses <= 0) {
        std::cerr << "Number of tosses should be a positive integer" << std::endl;
        return 1;
    }

    pthread_t threads[nthreads];
    struct ThreadArgs thread_args[nthreads];

    long long int tosses_per_thread = ceil((float)total_tosses / nthreads);
    long long int remaining_tosses = total_tosses;

    for (int t = 0; t < nthreads; t++) {
        thread_args[t] = {.id = t, .result = 0};
        thread_args[t].iterations = (t + 1 == nthreads) ? remaining_tosses : tosses_per_thread;
        if (pthread_create(&threads[t], NULL, toss, &thread_args[t])) {
            std::cerr << "Failed at pthread_create" << std::endl;
        }
        remaining_tosses -= tosses_per_thread;
    }

    for (int t = 0; t < nthreads; t++) {
        pthread_join(threads[t], NULL);
    }

    for (int t = 0; t < nthreads; t++) {
        number_in_circle += thread_args[t].result;
    }

    std::cout << std::setprecision(8) << 4 * number_in_circle / ((float)total_tosses) << std::endl;

    return 0;
}
