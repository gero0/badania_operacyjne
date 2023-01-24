#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <chrono>

using namespace std;

vector<vector<int>> load_tsp_file(string path, int problem_len);
vector<int> load_opt_tour(string path);
unsigned int path_len(const std::vector<int>& path, vector<vector<int>> dist_matrix);

vector<vector<int>> distance_matrix;
vector<vector<int>> cache;
vector<int> final_path;

/**************important stuff**************/

int tsp(int current, int included, int n)
{
    if (cache[current][included] != -1) {
        return cache[current][included];
    }
    //last
    if (included == (1 << (n - 1 - current))) {
        int d = distance_matrix[0][current];
        cache[current][included] = d;
        return d;
    }

    if ((included & (1 << (n - 1 - current))) == 0) {
        return INT_MAX;
    }

    int best_len = INT_MAX;
    int new_included = included & ~(1 << (n - 1 - current));

    for (int i = 1; i < n; i++) {
        if (i != current && included & (1 << (n - 1 - i))) {
            int result = distance_matrix[i][current] + tsp(i, new_included, n);
            best_len = min(best_len, result);
        }
    }

    cache[current][included] = best_len;
    return best_len;
}

void traverse(int current, int included, int n)
{
    if (included == (1 << (n - 1 - current))) {
        final_path.push_back(current);
        return;
    }

    int best_len = INT_MAX;
    int best_i = -1;

    int new_included = included & ~(1 << (n - 1 - current));
    for (int i = 1; i < n; i++) {
        if (i != current && included & (1 << (n - 1 - i))) {
            int result = distance_matrix[i][current] + tsp(i, new_included, n);
            if (result < best_len) {
                best_len = result;
                best_i = i;
            }
        }
    }

    final_path.push_back(current);
    traverse(best_i, new_included, n);
}

int main(int argc, char** argv)
{
    int problem_len = 0;
    if (argc > 2) {
        problem_len = std::stoi(argv[2]);
    }
    string path(argv[1]);
    distance_matrix = load_tsp_file(path, problem_len);
    auto opt_path = path.replace(path.find(".tsp"), 4, ".opt.tour");
    // auto opt_tour = load_opt_tour(opt_path);

    size_t perm_count = (1L << (distance_matrix.size() - 1));
    cache = vector<vector<int>>(distance_matrix.size(), vector<int>(perm_count, -1));

    auto all_included_mask = perm_count - 1;

    int best = INT_MAX;
    int best_i = -1;

    auto calc_start = std::chrono::high_resolution_clock::now();

    //calculate from bottom-up
    for (int i = 1; i < distance_matrix.size(); i++) {
        for (int mask = 1; mask <= all_included_mask; mask++) {
            int result = tsp(i, mask, distance_matrix.size()) + distance_matrix[i][0];
            if (mask == all_included_mask) {
                if (result < best) {
                    best = result;
                    best_i = i;
                }
            }
        }
    }

    final_path.push_back(0);
    traverse(best_i, all_included_mask, distance_matrix.size());
    final_path.push_back(0);
    reverse(final_path.begin(), final_path.end());

    auto calc_stop = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(calc_stop - calc_start).count();
    std::cout << "Time: " << time << " ms\n";

    // std::cout << "Opt len: " << path_len(opt_tour, distance_matrix) << "\n";
    std::cout << "Best len: " << best << "\n";
    std::cout << "Best i: " << best_i << "\n";
    std::cout << "PATH: ";
    for (auto e : final_path) {
        std::cout << e << " ";
    }
}

/**************************************** Parsers & helpers *******************************************************************************************************/

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

int dist(pair<float, float> p1, pair<float, float> p2)
{
    float xd = p2.first - p1.first;
    float yd = p2.second - p1.second;
    return (int)(std::sqrt(xd * xd + yd * yd));
}

unsigned int path_len(const std::vector<int>& path, vector<vector<int>> dist_matrix)
{
    unsigned int len = 0;
    for (int i = 0; i < path.size() - 1; i++) {
        len += dist_matrix[path[i]][path[i + 1]];
    }
    len += dist_matrix[path[path.size() - 1]][path[0]];
    return len;
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

vector<vector<int>> load_tsp_file(string path, int problem_len)
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

    if(problem_len != 0){
        dimension = problem_len;
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
            // return calc_dist_matrix(points);
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
