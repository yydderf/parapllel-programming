#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <climits>

double get_rand_between(long long int a, long long int b)
{
    return a + ((double)std::rand() / RAND_MAX) * (b - a);
}

int main(int argc, char **argv)
{
    std::srand(std::time(nullptr));

    long long int number_in_circle = 0;
    long long int total_tosses = 1000000;
    double x, y, squared_dist;

    for (int toss = 0; toss < total_tosses; toss++) {
        x = get_rand_between(0, 1);
        y = get_rand_between(0, 1);
        squared_dist = x * x + y * y;
        if (squared_dist <= 1) {
            number_in_circle += 1;
        }
    }

    std::cout << 4 * number_in_circle / ((double)total_tosses) << std::endl;

    return 0;
}
