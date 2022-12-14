#include "tsp_helpers.h"
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
    std::random_device rd;
    std::mt19937 ugen(rd());
    std::uniform_real_distribution<> uniform(0.0, 1.0);

    srand(time(NULL));
    vector<Point> points;

    load_tsp_file(argv[1], points);
    int max_iter = std::stoi(argv[2]);
    int max_iter_per_epoch = std::stoi(argv[3]);

    int Tmax = std::stoi(argv[4]);
    int Tmin = std::stoi(argv[5]);

    float T = Tmax;

    auto tour = alg_123(points);

    print_tour(tour);

    for (int i = 0; i < max_iter && T >= Tmin; i++) {
        for (int j = 0; j < max_iter_per_epoch; j++) {
            // std::uniform_int_distribution<> distr(0, tour.size() - 3);
            // int a = distr(ugen);
            // std::uniform_int_distribution<> distr2(a + 1, tour.size() - 2);
            // int b = distr(ugen);

            std::uniform_int_distribution<> distr(0, tour.size() - 2);
            int a = distr(ugen);
            int b = distr(ugen);

            if(a == b) continue;

            float diff = dist(tour[a], tour[b]) + dist(tour[a + 1], tour[b + 1])
                - dist(tour[a], tour[a + 1]) - dist(tour[b], tour[b + 1]);

            //worse solution
            if (diff > 0) {
                float U_random = uniform(ugen);
                float treshold = std::exp(-diff / T);
                //continue loop, don't accept worse solution
                // std::cout << "A: " << a << " B: " << b << "\n";
                std::cout << "TEMP: " << T << " DISTANCE: " << diff << " URANDOM: " << U_random << " TRESHOLD: " << treshold << "\n";
                if (U_random > treshold) {
                    // std::cout << "REJECTED\n";
                    continue;
                }
                // std::cout << "ACCEPTED\n";
            }

            std::reverse(tour.begin() + a + 1, tour.begin() + b + 1);
        }

        T = 0.99 * T;

        print_tour(tour);
    }
}