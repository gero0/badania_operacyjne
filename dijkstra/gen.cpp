#include "jngen.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>

int main(int argc, char** argv)
{
    int N = std::stoi(argv[1]);
    int M = std::stoi(argv[2]);

    auto graph = jngen::Graph::random(N, M).connected(true);
    std::stringstream ss;
    std::ostringstream output;
    ss << graph;

    srand(time(NULL));

    output << N << "\t" << (2 * M) << "\n";

    for (int i = 0; i < M; i++) {
        int a,b;
        ss >> a >> b;
        ++a;
        ++b;
        int w = (rand() % 20) + 1;
        output << a << "\t" << b << "\t" << w << "\n";
        output << b << "\t" << a << "\t" << w << "\n";
    }

    std::cout << output.str();
}
