#include "tsp_helpers.h"
#include "climits"

unsigned int min_dist(const std::vector<Point>& path)
{
    unsigned int d_min = INT_MAX;
    for (int i = 0; i < path.size(); i++) {
        for(int j = 0; j < path.size(); j++){
            if(i == j) continue;
            auto d = dist(path[i], path[j]);
            if(d < d_min) {
                d_min = d;
            }
        }
    }
    return d_min;
}

unsigned int max_dist(const std::vector<Point>& path)
{
    unsigned int d_max = 0;
    for (int i = 0; i < path.size(); i++) {
        for(int j = 0; j < path.size(); j++){
            if(i == j) continue;
            auto d = dist(path[i], path[j]);
            if(d > d_max) {
                d_max = d;
            }
        }
    }
    return d_max;
}

int main(int argc, char** argv){
    std::vector<Point> points;
    load_tsp_file(argv[1], points);

    std::cout << "Minimum distance is:" << min_dist(points) << "\n";
    std::cout << "Maximum distance is:" << max_dist(points) << "\n";
}