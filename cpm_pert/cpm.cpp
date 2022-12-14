#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

int N;
int M;

std::vector<std::vector<int>> A;

std::vector<int> ES;
std::vector<int> EF;
std::vector<int> LS;
std::vector<int> LF;
std::vector<int> TF;

std::vector<int> t;

int max_EF = 0;

bool calculate_ef_es(int, int);
void calculate_lf_ls_tf(int);
void load_file(const char* path);
void search_cp(int, std::vector<int>, int);

void prep(std::vector<int>& v)
{
    v.resize(N);
    std::fill(v.begin(), v.end(), -1);
}

int main()
{
	//load data from file
    load_file("data80.txt");

	//fill vectors with -1
    prep(ES);
    prep(EF);
    prep(LS);
    prep(LF);
    prep(TF);

	//calculate EF and ES, exit if graph has a cycle
    for (int i = 0; i < N; i++) {
        bool success = calculate_ef_es(i, 0);
        if (!success) {
            printf("Error, graph has a cycle");
			return -1;
        }
    }

	//calculate LF, LS and TF
    for (int i = 0; i < N; i++) {
        calculate_lf_ls_tf(i);
    }

	//Copy indexes of all tasks with TF == 0 to crirical path vector
	std::vector<int> cp;
    for (int i = 0; i < N; i++) {
        if (TF[i] == 0) {
            cp.push_back(i);
        }
    }

	//Sort tasks in critical path chronologically
    std::sort(cp.begin(), cp.end(), [](int a, int b) { return ES[a] < ES[b]; });

	//Print results
    printf("Process time: %d\nnode\tES\tEF\tLS\tLF\n", max_EF);

    for (int i = 0; i < N; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n", i, ES[i], EF[i], LS[i], LF[i]);
    }

    printf("CRITICAL PATH: \n");
    for (int a : cp) {
        printf("%d\t%d\t%d\n", a + 1, LS[a], LF[a]);
    }

    return 0;
}

void load_file(const char* path)
{
    std::ifstream file;
    file.open(path);

    file >> N >> M;

    //prepare matrix
    for (int i = 0; i < N; i++) {
        A.push_back(std::vector<int>(N, 0));
    }

    for (int i = 0; i < N; i++) {
        int node;
        file >> node;
        t.push_back(node);
    }

    for (int i = 0; i < M; i++) {
        int x, y;
        file >> x >> y;
        A[x - 1][y - 1] = 1;
    }

    file.close();
}


bool calculate_ef_es(int node, int recursion_depth)
{
    //cycle detected
    if (recursion_depth >= N) {
        return false;
    }

    //value already calculated
    if (EF[node] != -1) {
        return true;
    }

    ES[node] = 0;

    for (int i = 0; i < N; i++) {
        //don't check if node has path to itself
        if (i == node)
            continue;

        //there is an edge from node i to this node
        if (A[i][node] == 1) {
            bool success = calculate_ef_es(i, recursion_depth + 1);
            if (!success)
                return false;

            if (EF[i] > ES[node]) {
                ES[node] = EF[i];
            }
        }

        EF[node] = ES[node] + t[node];
        if (EF[node] > max_EF) {
            max_EF = EF[node];
        }
    }

    return true;
}

void calculate_lf_ls_tf(int node)
{
    //value already calculated
    if (LS[node] != -1) {
        return;
    }

    LF[node] = max_EF;

    for (int i = 0; i < N; i++) {
        if (i == node)
            continue;

        if (A[node][i] == 1) {
            calculate_lf_ls_tf(i);

            if (LS[i] < LF[node]) {
                LF[node] = LS[i];
            }
        }
    }

    LS[node] = LF[node] - t[node];
    TF[node] = LS[node] - ES[node];
}