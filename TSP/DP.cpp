#include "tsp_helpers.h"
#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

int tsp(int visited, int current, int completed, const vector<vector<int>>& distance_matrix, vector<vector<int>>& cache)
{
    if (visited == completed) {
        int d = distance_matrix[current][0];
        return d;
    }

    if (cache[visited][current] != -1) {
        return cache[visited][current];
    }

    int best_len = INT_MAX;

    for (int i = 0; i < distance_matrix.size(); i++) {
        if (i == current)
            continue;

        if (visited & (1 << i)) {
            //node visited, skip
            continue;
        }

        //set as visited
        int new_visited = visited | (1 << i);
        int result = distance_matrix[current][i] + tsp(new_visited, i, completed, distance_matrix, cache);
        if (best_len > result) {
            best_len = result;
        }
    }

    cache[visited][current] = best_len;
    return best_len;
}

vector<vector<int>> calc_dist_matrix(std::vector<Point> points)
{
    auto distance_matrix = vector<vector<int>>(points.size(), vector<int>(points.size(), 0));
    for (int i = 0; i < points.size(); i++) {
        for (int j = 0; j < points.size(); j++) {
            distance_matrix[i][j] = dist(points[i], points[j]);
        }
    }
    return distance_matrix;
}

int main(int argc, char** argv)
{
    vector<Point> points;
    vector<Point> opt_tour;
    load_tsp_file(argv[1], points);
    std::string path(argv[1]);
    auto opt_path = path.replace(path.find(".tsp"), 4, ".opt.tour");
    load_opt_tour(opt_path, points, opt_tour);

    int completed = (1 << points.size()) - 1;
    auto cache = vector<vector<int>>(completed, vector<int>(points.size(), -1));
    auto distance_matrix = calc_dist_matrix(points);

    cout << "Minimum Distance: " << tsp(1, 0, completed, distance_matrix, cache);
}