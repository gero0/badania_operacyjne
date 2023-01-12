#include "tsp_helpers.h"
#include <chrono>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

vector<vector<int>> cache;
vector<vector<int>> cache_next_node;
vector<vector<int>> distance_matrix;

int tsp(int current, long included_nodes)
{
    if ((included_nodes & (1 << current)) == 0) {
        //path from x that doesnt include x - impossible
        return INT_MAX;
    }

    if (included_nodes == (1 << current)) {
        //only current node is in path - return distance to starting node
        cache_next_node[included_nodes][current] = 0;
        return distance_matrix[current][0];
    }

    if (cache[included_nodes][current] != -1) {
        //cache hit?
        return cache[included_nodes][current];
    }

    int best_len = INT_MAX;
    int best_node = -1;

    for (int i = 1; i < distance_matrix.size(); i++) {
        //if skip if i is current node or is not included in the path
        if (i == current || (included_nodes & (1 << i)) == 0)
            continue;

        //remove current from mask
        int new_included_nodes = included_nodes & ~(1 << current);
        int result = distance_matrix[current][i] + tsp(i, new_included_nodes);
        if (best_len > result) {
            best_len = result;
            best_node = i;
        }
    }

    cache[included_nodes][current] = best_len;
    cache_next_node[included_nodes][current] = best_node;
    return best_len;
}

int traverse(long included, int current){
    if(current == 0){
        return current;
    }
    // vector<int> path;
    int next_node = cache_next_node[included][current];
    long new_visited = included & ~(1 << current);
    std::cout << traverse(new_visited, next_node) + 1 << " ";
    return current;
}

vector<vector<int>> calc_dist_matrix(std::vector<Point> points)
{
    auto distance_matrix = vector<vector<int>>(points.size(), vector<int>(points.size(), 0));
    for (int i = 0; i < points.size(); i++) {
        for (int j = 0; j < points.size(); j++) {
            if (i == j)
                continue;
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

    size_t perm_count = (1L << points.size()) - 1;
    cache = vector<vector<int>>(perm_count, vector<int>(points.size(), -1));
    cache_next_node = vector<vector<int>>(perm_count, vector<int>(points.size(), -1));
    distance_matrix = calc_dist_matrix(points);

    long all_visited_mask = (1 << points.size()) - 2;

    int best_len = INT_MAX;
    int best_node = 0;

    //by incrementing by 2 last bit (starting point) will always be 0
    //start from bottom-up
    for (size_t mask = 2; mask <= all_visited_mask; mask += 2) {
        for (int i = 1; i < points.size(); i++) {
            int result = tsp(i, mask);
            if(mask == all_visited_mask){
                if (best_len > result){
                    best_len = result + dist(points[0], points[i]) ;
                    best_node = i;
                }
            }
        }
    }

    std::cout << traverse(all_visited_mask, best_node) + 1 << "\n";

    // for (auto v : distance_matrix) {
    //     for (auto node : v) {
    //         std::cout << node << " ";
    //     }
    //     std::cout << "\n";
    // }

    // for (auto v : cache) {
    //     for (auto node : v) {
    //         std::cout << node << " ";
    //     }
    //     std::cout << "\n";
    // }

    // for (auto v : cache_next_node) {
    //     for (auto node : v) {
    //         std::cout << node << " ";
    //     }
    //     std::cout << "\n";
    // }

    // do {
    //     std::cout << best_node << " ";
    //     best_node = cache_next_node[completed][best_node];
    // }while(best_node != -1);
    cout << "Best node : " << best_node << "\n";
    cout << "Minimum Distance: " << best_len << "\n";
    cout << "Opt:" << path_len(opt_tour);
}