#include "tsp_helpers.h"
#include <chrono>
#include <climits>
#include <cstdlib>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

vector<vector<int>> cache;
vector<vector<int>> cache_next_node;
vector<vector<int>> distance_matrix;

unsigned int path_len(const std::vector<int>& path, vector<vector<int>> dist_matrix)
{
    unsigned int len = 0;
    for (int i = 0; i < path.size() - 1; i++) {
        len += dist_matrix[path[i]][path[i + 1]];
    }
    len += dist_matrix[path[path.size() - 1]][path[0]];
    return len;
}

float to_rad(float x)
{
    float PI = 3.141592;
    int deg = (int)(x);
    float m = x - deg;
    return PI * ((float)deg + 5.0 * m / 3.0) / 180.0;
}

int geo_dist(pair<float, float> p1, pair<float, float> p2)
{
    float lat1 = to_rad(p1.first);
    float lat2 = to_rad(p2.first);
    float long1 = to_rad(p1.second);
    float long2 = to_rad(p2.second);

    float RRR = 6378.388;
    float q1 = cos(long1 - long2);
    float q2 = cos(lat1 - lat2);
    float q3 = cos(lat1 + lat2);

    float result = RRR * acos(0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3)) + 1.0;
    return (int)(result);
}

int dist(pair<float, float> p1, pair<float, float> p2)
{
    int xd = p2.first - p1.first;
    int yd = p2.second - p1.second;
    return (int)(0.5f + std::sqrt(xd * xd + yd * yd));
}

vector<vector<int>> calc_dist_matrix(vector<pair<float, float>> points)
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

vector<vector<int>> calc_geo_matrix(vector<pair<float, float>> points)
{
    auto distance_matrix = vector<vector<int>>(points.size(), vector<int>(points.size(), 0));
    for (int i = 0; i < points.size(); i++) {
        for (int j = 0; j < points.size(); j++) {
            if (i == j)
                continue;
            distance_matrix[i][j] = geo_dist(points[i], points[j]);
        }
    }
    return distance_matrix;
}

string trim(const string& s)
{
    auto start = s.begin();
    while (start != s.end() && isspace(*start)) {
        start++;
    }

    auto end = s.end();
    do {
        end--;
    } while (distance(start, end) > 0 && isspace(*end));

    return string(start, end + 1);
}

vector<string> tokenize(string str, char delimiter)
{
    vector<string> tokens;
    string segment;
    stringstream ss(str);
    while (getline(ss, segment, ':')) {
        segment = trim(segment);
        tokens.push_back(segment);
    }

    return tokens;
}

vector<int> load_opt_tour(string path)
{
    fstream file;
    file.open(path);
    string line;
    vector<int> tour;

    int dimension = 0;
    do {
        getline(file, line);
        auto tokens = tokenize(line, ':');
        if (tokens[0] == "DIMENSION") {
            dimension = std::stoi(tokens[1]);
        }
    } while (line != "TOUR_SECTION");

    if (dimension == 0) {
        throw runtime_error("No dimension given in input file!");
    }

    for (int i = 0; i < dimension; i++) {
        int id;
        file >> id;
        tour.push_back(id - 1);
    }

    return tour;
}

vector<vector<int>> load_tsp_file(string path)
{
    fstream file;
    file.open(path);

    string weight_type("EUC_2D");
    string input_type("FUNCTION");
    int dimension = 0;

    string line;
    do {
        getline(file, line);
        auto tokens = tokenize(line, ':');
        if (tokens[0] == "DIMENSION") {
            dimension = std::stoi(tokens[1]);
        } else if (tokens[0] == "EDGE_WEIGHT_FORMAT") {
            input_type = tokens[1];
        } else if (tokens[0] == "EDGE_WEIGHT_TYPE") {
            weight_type = tokens[1];
        }
    } while (line != "NODE_COORD_SECTION" && line != "EDGE_WEIGHT_SECTION");

    if (dimension == 0) {
        throw runtime_error("No dimension given in input file!");
    }

    if (input_type == "FUNCTION") {
        vector<pair<float, float>> points;

        for (int i = 0; i < dimension; i++) {
            int id;
            float x, y;
            file >> id >> x >> y;
            points.push_back({ x, y });
        }

        if (weight_type == "EUC_2D") {
            return calc_dist_matrix(points);
        } else if (weight_type == "GEO") {
            return calc_geo_matrix(points);
        } else {
            throw runtime_error("Unsupported input format!");
        }

    } else if (input_type == "FULL_MATRIX") {
        vector<vector<int>> dist_matrix(dimension, vector<int>(dimension, 0));
        for (int row = 0; row < dimension; row++) {
            for (int col = 0; col < dimension; col++) {
                int value;
                file >> value;
                dist_matrix[row][col] = value;
            }
        }

        return dist_matrix;
    } else {
        throw runtime_error("Unsupported input format!");
    }
}

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

int traverse(long included, int current)
{
    if (current == 0) {
        return current;
    }
    // vector<int> path;
    int next_node = cache_next_node[included][current];
    long new_visited = included & ~(1 << current);
    cout << traverse(new_visited, next_node) + 1 << " ";
    return current;
}

int main(int argc, char** argv)
{
    string path(argv[1]);
    distance_matrix = load_tsp_file(path);
    auto opt_path = path.replace(path.find(".tsp"), 4, ".opt.tour");
    auto opt_tour = load_opt_tour(opt_path);


    size_t perm_count = (1L << distance_matrix.size()) - 1;
    cache = vector<vector<int>>(perm_count, vector<int>(distance_matrix.size(), -1));
    cache_next_node = vector<vector<int>>(perm_count, vector<int>(distance_matrix.size(), -1));

    long all_visited_mask = (1 << distance_matrix.size()) - 2;

    int best_len = INT_MAX;
    int best_node = 0;

    //by incrementing by 2 last bit (starting point) will always be 0
    //start from bottom-up
    for (size_t mask = 2; mask <= all_visited_mask; mask += 2) {
        for (int i = 1; i < distance_matrix.size(); i++) {
            int result = tsp(i, mask);
            if (mask == all_visited_mask) {
                if (best_len > result) {
                    best_len = result + distance_matrix[0][i];
                    best_node = i;
                }
            }
        }
    }

    cout << traverse(all_visited_mask, best_node) + 1 << "\n";

    // for (auto v : distance_matrix) {
    //     for (auto node : v) {
    //         cout << node << " ";
    //     }
    //     cout << "\n";
    // }

    // for (auto v : cache) {
    //     for (auto node : v) {
    //         cout << node << " ";
    //     }
    //     cout << "\n";
    // }

    // for (auto v : cache_next_node) {
    //     for (auto node : v) {
    //         cout << node << " ";
    //     }
    //     cout << "\n";
    // }

    // do {
    //     cout << best_node << " ";
    //     best_node = cache_next_node[completed][best_node];
    // }while(best_node != -1);
    cout << "Best node : " << best_node << "\n";
    cout << "Minimum Distance: " << best_len << "\n";
    cout << "Opt:" << path_len(opt_tour, distance_matrix);
}