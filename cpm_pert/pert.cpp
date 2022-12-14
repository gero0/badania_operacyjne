#include <algorithm>
#include <cmath>
#include <cstdio>
#include <vector>

const int N = 9;

std::vector<std::vector<int>> A = {
    { 0, 0, 1, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 1, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 1, 1, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

std::vector<int> a = { 1, 2, 1, 1, 3, 2, 1, 4, 5 };
std::vector<int> m = { 2, 3, 2, 2, 4, 4, 3, 5, 7 };
std::vector<int> b = { 3, 4, 3, 3, 5, 6, 5, 7, 9 };

std::vector<int> t;
std::vector<double> variance;

std::vector<int> ES;
std::vector<int> EF;
std::vector<int> LS;
std::vector<int> LF;
std::vector<int> TF;

int max_EF = 0;
int max_EF_i = 0;

bool calculate_ef_es(int, int);
void calculate_lf_ls_tf(int);

double cdf(double x)
{
    return std::erfc(-x / sqrt(2)) / 2;
}

//based on
//https://www.researchgate.net/publication/233917902
double invcdf(double p)
{
    if (p < 0.5) {
        double LR = p / (1 - p);
        return 5.5310 * (std::pow(LR, 0.1193) - 1);
    }

    double LR = (1 - p) / p;
    return -5.5310 * (std::pow(LR, 0.1193) - 1);
}

template <typename T>
void prep(std::vector<T>& v)
{
    v.resize(N);
    std::fill(v.begin(), v.end(), -1);
}

int main()
{ //load data from file
    // load_file("data80.txt");

    //fill vectors with -1
    prep(ES);
    prep(EF);
    prep(LS);
    prep(LF);
    prep(TF);
    prep(t);
    prep(variance);

    //calculate t_oper and variance
    for (int i = 0; i < N; i++) {
        t[i] = (a[i] + 4 * m[i] + b[i]) / 6;
        variance[i] = std::pow((double)(b[i] - a[i]) / 6.0, 2.0);
    }

    //forward
    for (int i = 0; i < N; i++) {
        bool success = calculate_ef_es(i, 0);
        if (!success) {
            printf("ERROR: Recursion max depth exceeded, the graph has cycles!");
            return -1;
        }
    }

    //backward
    for (int i = 0; i < N; i++) {
        calculate_lf_ls_tf(i);
    }

    //calculate mean and standard deviation
    int mu = 0;
    double var = 0.0;
    for (int i = 0; i < N; i++) {
        if (TF[i] == 0) {
            mu += t[i];
            var += variance[i];
        }
    }

    double std_dev = std::sqrt(var);

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

    printf("\nMean: %d Std. Dev: %f\n", mu, std_dev);

    double p = cdf((17 - mu) / std_dev);
    printf("p(t<17) = %f \n", p);

    printf("t(p = 0.99): %f\n", mu + std_dev * invcdf(0.99));

    return 0;
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