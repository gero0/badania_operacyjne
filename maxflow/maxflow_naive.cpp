#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
using Mat = vector<vector<int>>;

int N;
Mat C; //graph weights ( capacity)
Mat F; //flow
Mat R; //residual
vector<pair<int, vector<int>>> paths;
vector<pair<int, vector<int>>> chosen_paths;

void load_data(const char* path);
vector<int> dijkstra(int source, int dst);
void sub_mat(Mat A, Mat B, Mat& C, int n);
void check_edges(Mat& E, Mat R);

void find_path(std::vector<int> current_path, int node, int target)
{
    for (auto n : current_path) {
        //cycle
        if (n == node)
            return;
    }
    current_path.push_back(node);
    if (node == target) {
        int min_cap = INT_MAX;
        for (int i = 0; i < current_path.size() - 1; i++) {
            int x = current_path[i];
            int y = current_path[i + 1];
            if (R[x][y] < min_cap) {
                min_cap = R[x][y];
            }
        }

        paths.push_back({ min_cap, current_path });
        return;
    }
    for (int i = 0; i < N; i++) {
        if (R[node][i] != 0) {
            find_path(current_path, i, target);
        }
    }
}

int main(int argc, char** argv)
{
    load_data(argv[1]);
    int source = 0;
    int dst = N - 1;

    auto calc_start = std::chrono::high_resolution_clock::now();

    while (true) {
        sub_mat(C, F, R, N);
        find_path(std::vector<int>(), source, dst);

        if (paths.size() < 1)
            break;

        auto path = paths[0];
        for (int i = 0; i < path.second.size() - 1; i++) {
            int x = path.second[i];
            int y = path.second[i + 1];
            F[x][y] = F[x][y] + path.first;
            F[y][x] = F[y][x] - path.first;
        }

        chosen_paths.push_back(path);
        paths.clear();
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

    std::reverse(chosen_paths.begin(), chosen_paths.end());

    std::cout << "Ścieżki przepływów:\n";
    for (auto p : chosen_paths) {
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

void load_data(const char* path)
{
    std::ifstream file;
    file.open(path);

    file >> N;

    //prepare matrix
    for (int i = 0; i < N; i++) {
        C.push_back(std::vector<int>(N, 0));
        F.push_back(std::vector<int>(N, 0));
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