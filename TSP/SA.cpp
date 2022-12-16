#include "tsp_helpers.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
    std::random_device rd;
    std::mt19937 ugen(rd());
    std::uniform_real_distribution<> uniform(0.0, 1.0);

    vector<Point> points;
    vector<Point> opt_tour;

    load_tsp_file(argv[1], points);
    std::string path(argv[1]);
    auto opt_path = path.replace(path.find(".tsp"), 4, ".opt.tour");
    load_opt_tour(opt_path, points, opt_tour);

    int max_iter = std::stoi(argv[2]);
    int max_iter_per_epoch = std::stoi(argv[3]);

    int Tmax = std::stoi(argv[4]);
    int Tmin = std::stoi(argv[5]);

    float T = Tmax;

    auto tour = alg_123(points);

    int curr_path_len = path_len(tour);
    int opt_path_len = path_len(opt_tour);

    print_tour(tour);

    int it_n = 1;

    #ifdef SAVE_HIST
    Logger logger("./SA_hist.csv");
    #endif

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
                // std::cout << "TEMP: " << T << " DISTANCE: " << diff << " URANDOM: " << U_random << " TRESHOLD: " << treshold << "\n";
                if (U_random > treshold) {
                    // std::cout << "REJECTED\n";
                    continue;
                }
                // std::cout << "ACCEPTED\n";
            }
            // std::cout << "TEMP: " << T << " DISTANCE: " << diff << "\n";

            #ifdef SAVE_HIST
            curr_path_len = path_len(tour);
            #endif
            std::reverse(tour.begin() + a + 1, tour.begin() + b + 1);
        }

        T = 0.99 * T;

        #ifdef SAVE_HIST
        float gap = calc_gap(curr_path_len, opt_path_len);
        logger.log(it_n, curr_path_len, opt_path_len, gap);
        it_n++;
        #endif
        print_tour(tour);
    }
}