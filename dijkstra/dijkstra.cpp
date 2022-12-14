#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

unordered_set<int> not_done;

vector<vector<int>> E;
vector<int> dist;
vector<int> previous;
int target;
int N, M;

void load_data(const char* path)
{
    std::ifstream file;
    file.open(path);

    file >> N >> M;
    std::cout << N << ":" << M << "\n";

    //prepare matrix
    for (int i = 0; i < N; i++) {
        E.push_back(std::vector<int>(N, -1));
    }

    for (int i = 0; i < M; i++) {
        int x, y, w;
        file >> x >> y >> w;
        E[x - 1][y - 1] = w;
    }

    dist = std::vector<int>(N, INT_MAX);
    dist[0] = 0;
    previous = std::vector<int>(N, -1);

    file.close();
}

int main(int argc, char** argv)
{
    load_data(argv[1]);

    target = N - 1;

    auto calc_start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        not_done.insert(i);
    }

    while (!not_done.empty()) {
        int min_index = 0;
        int max_val = INT_MAX;

        for (auto e : not_done) {
            if (dist[e] < max_val) {
                min_index = e;
                max_val = dist[e];
            }
        }

        not_done.erase(min_index);

        for (int i = 0; i < N; i++) {
            if (E[min_index][i] > -1 && (long)dist[min_index] + E[min_index][i] < dist[i]) {
                dist[i] = dist[min_index] + E[min_index][i];
                previous[i] = min_index;
            }
        }
    }

    auto calc_stop = std::chrono::high_resolution_clock::now();

    std::vector<int> path;
    int node = target;
    while (node >= 0) {
        path.push_back(node);
        node = previous[node];
    }

    for (int i = 0; i < N; i++) {
        std::cout << i << ": " << dist[i] << " " << previous[i] << "\n";
    }

    std::cout << "Path to target:"
              << "\n";

    for (int i = path.size() - 1; i >= 0; i--) {
        std::cout << path[i] << " ";
    }

    std::cout << "\nCalculating path lengths took: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(calc_stop - calc_start).count() << "ms\n";

    return 0;
}