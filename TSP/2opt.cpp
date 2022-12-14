#include "tsp_helpers.h"
#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
    vector<Point> points;

    load_tsp_file(argv[1], points);

    auto tour = alg_123(points);

    bool improvement = true;

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

        print_tour(tour);
    }
}