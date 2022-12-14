#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;
using Mat = vector<vector<int>>;

int N;
Mat C; //graph weights ( capacity)
Mat F; //flow
Mat R; //residual
Mat E;
vector<pair<int, vector<int>>> paths;

void load_data(const char* path);
vector<int> dijkstra(int source, int dst);
void sub_mat(Mat A, Mat B, Mat& C, int n);
void check_edges(Mat& E, Mat R);

int main(int argc, char** argv)
{
    load_data(argv[1]);
    int source = 0;
    int dst = N - 1;

    auto calc_start = std::chrono::high_resolution_clock::now();

    while (true) {
        sub_mat(C, F, R, N);
        check_edges(E, R);

        auto path = dijkstra(source, dst);
        if (path.size() < 1) {
            break;
        }

        std::vector<int> r;
        for (int i = 0; i < path.size() - 1; i++) {
            int x = path[i];
            int y = path[i + 1];
            r.push_back(R[x][y]);
        }

        int a = *std::min_element(r.begin(), r.end());

        for (int i = 0; i < path.size() - 1; i++) {
            int x = path[i];
            int y = path[i + 1];
            F[x][y] = F[x][y] + a;
            F[y][x] = F[y][x] - a;
        }

        paths.push_back(make_pair(a, path));
    }

    auto calc_stop= std::chrono::high_resolution_clock::now();

    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            F[row][col] = F[row][col] > 0 ? F[row][col] : 0;
        }
    }

    vector<int> sums(N, -1);
    for (int i = 0; i < F.size(); i++) {
        int sum = 0;
        for (auto col : F[i]) {
            sum += col;
        }
        sums[i] = sum;
    }

    std::cout << "Maksymalny przepływ: " << *std::max_element(sums.begin(), sums.end()) << "\n";

    std::cout << "Graf przepływów:\n";
    for (int i = 0; i < F.size(); i++) {
        std::cout << sums[i] << ": ";
        for (auto col : F[i]) {
            std::cout << col << " ";
        }
        std::cout << "\n";
    }

    std::cout << "Ścieżki przepływów:\n";
    for (auto p : paths) {
        int a = p.first;
        std::cout << a << ": ";
        for (auto e : p.second) {
            std::cout << e << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\nTIME: "
              << std::chrono::duration_cast<std::chrono::microseconds>(calc_stop - calc_start).count() << "us\n";
}

void sub_mat(Mat A, Mat B, Mat& C, int n)
{
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            C[row][col] = A[row][col] - B[row][col];
        }
    }
}

void check_edges(Mat& E, Mat R)
{
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            E[row][col] = (R[row][col] != 0);
        }
    }
}

vector<int> dijkstra(int source, int dst)
{
    std::unordered_set<int> not_done;
    std::vector<int> dist(N, INT_MAX);
    dist[source] = 0;
    std::vector<int> previous(N, -1);

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

        //path does not exist, return empty vector
        if (max_val == INT_MAX) {
            return std::vector<int>();
        }

        not_done.erase(min_index);

        for (int i = 0; i < N; i++) {
            if (E[min_index][i] > 0 && (long)dist[min_index] + E[min_index][i] < dist[i]) {
                dist[i] = dist[min_index] + E[min_index][i];
                previous[i] = min_index;
            }
        }
    }

    std::vector<int> path;
    int node = dst;
    while (node >= 0) {
        path.push_back(node);
        node = previous[node];
    }

    reverse(path.begin(), path.end());
    return path;
}

void load_data(const char* path)
{
    std::ifstream file;
    file.open(path);

    file >> N;

    //prepare matrix
    for (int i = 0; i < N; i++) {
        C.push_back(std::vector<int>(N, 0));
        F.push_back(std::vector<int>(N, 0));
        E.push_back(std::vector<int>(N, 0));
        R.push_back(std::vector<int>(N, 0));
    }

    for (int i = 0; i < N * N; i++) {
        int n;
        file >> n;
        int row = i / N;
        int column = i % N;
        C[row][column] = n;
    }

    file.close();
}