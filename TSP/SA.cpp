#include "tsp_helpers.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <unistd.h>

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

    int splits = std::stoi(argv[2]);
    int max_iter_per_epoch = std::stoi(argv[3]);

    int Tmax = std::stoi(argv[4]);
    int Tmin = std::stoi(argv[5]);

    float T = Tmax;

    auto tour = alg_123(points);

    int curr_path_len = path_len(tour);
    int opt_path_len = path_len(opt_tour);

    print_tour(tour);

    int it_n = 1;

    float temp_factor = std::pow(float(Tmin) / Tmax, 1.0 / (splits - 1));
    std::cout << "Temp. decrease factor: " << temp_factor << "\n";

    auto calc_start = std::chrono::high_resolution_clock::now();

#ifdef SAVE_HIST
    Logger logger("./SA_hist.csv");
#endif

    for (int i = 0; T > Tmin; i++) {
        for (int j = 0; j < max_iter_per_epoch; j++) {
            std::uniform_int_distribution<> distr(0, tour.size() - 2);
            int a = distr(ugen);
            int b = distr(ugen);

            if (a == b)
                continue;

            float diff = dist(tour[a], tour[b]) + dist(tour[a + 1], tour[b + 1])
                - dist(tour[a], tour[a + 1]) - dist(tour[b], tour[b + 1]);

            //worse solution
            if (diff > 0) {
                float U_random = uniform(ugen);
                float treshold = std::exp(-diff / T);
                if (U_random > treshold) {
                    // #ifdef SAVE_HIST
                    //                     float gap = calc_gap(curr_path_len, opt_path_len);
                    //                     logger.log(it_n, curr_path_len, opt_path_len, gap);
                    // #endif
                    it_n++;
                    continue;
                }
            }

            std::reverse(tour.begin() + a + 1, tour.begin() + b + 1);

#ifdef SAVE_HIST
            curr_path_len = path_len(tour);
            float gap = calc_gap(curr_path_len, opt_path_len);
            logger.log(it_n, curr_path_len, opt_path_len, gap);
            it_n++;
#endif
        }
        // std::cout << "TEXT T:" << T << "\n";
        // print_tour(tour);
        T = temp_factor * T;
    }

    auto calc_stop = std::chrono::high_resolution_clock::now();

    print_tour(tour);
    curr_path_len = path_len(tour);
    float gap = calc_gap(curr_path_len, opt_path_len);
    std::cout << "FINAL GAP: " << gap << "% FINAL TOUR LENGTH: " << curr_path_len << " OPT. PATH LENGTH: " << opt_path_len << "\n";
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(calc_stop - calc_start).count();
    std::cout << "Time: " << time << " s";
}