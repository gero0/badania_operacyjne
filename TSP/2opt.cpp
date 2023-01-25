#include "tsp_helpers.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>

using namespace std;

int main(int argc, char** argv)
{
    vector<Point> points;
    vector<Point> opt_tour;

    load_tsp_file(argv[1], points);
    std::string path(argv[1]);
    auto opt_path = path.replace(path.find(".tsp"), 4, ".opt.tour");
    std::cout << opt_path << "\n";
    load_opt_tour(opt_path, points, opt_tour);

    auto tour
        = alg_123(points);

    int curr_path_len = path_len(tour);
    int opt_path_len = path_len(opt_tour);

#ifdef SAVE_HIST
    Logger logger("./2opt_hist.csv");
#endif

    bool improvement = true;

    int it_n = 1;

    auto calc_start = std::chrono::high_resolution_clock::now();

    while (improvement) {
        improvement = false;
        int min_dist = 0;
        //search for pair
        int a = 0, b = 0;

        for (int i = 0; i < tour.size() - 2; i++) {
            for (int j = i + 1; j < tour.size() - 1; j++) {
                int distance = dist(tour[i], tour[j]) + dist(tour[i + 1], tour[j + 1])
                    - dist(tour[i], tour[i + 1]) - dist(tour[j], tour[j + 1]);

                if (distance < min_dist) {
                    // std::cout << distance << "\n";
                    min_dist = distance;
                    a = i;
                    b = j;
                    improvement = true;
                }
            }
        }

        if (!improvement)
            break;

        std::reverse(tour.begin() + a + 1, tour.begin() + b + 1);

        curr_path_len += min_dist;
#ifdef SAVE_HIST
        float gap = calc_gap(curr_path_len, opt_path_len);
        logger.log(it_n, curr_path_len, opt_path_len, gap);
        it_n++;
#endif

        // print_tour(tour);
        // print_tour(opt_tour);
    }

    auto calc_stop = std::chrono::high_resolution_clock::now();
    
    float gap = calc_gap(curr_path_len, opt_path_len);
    std::cout << "FINAL GAP: " << gap << "% FINAL TOUR LENGTH: " << curr_path_len << " OPT. PATH LENGTH: " << opt_path_len << "\n";
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(calc_stop - calc_start).count();
    std::cout << "Time: " << time << " us";
}