#include <algorithm>
#include <cfloat>
#include <cmath>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include "tsp_helpers.h"

using namespace std;

size_t find_furthest_index(const vector<Point>& in_path, const vector<Point>& not_in_path)
{
    float furthest_distance = 0.0f;
    int furthest_point = 0;

    for (int i = 0; i < not_in_path.size(); i++) {
        Point point = not_in_path[i];
        //Find the minimal distance between this point and any point in path
        float min_distance = FLT_MAX;
        for (Point point_in_path : in_path) {
            float distance = dist(point, point_in_path);
            if (distance < min_distance)
                min_distance = distance;
        }
        //Find the furthest from the points not in path
        if (min_distance > furthest_distance) {
            furthest_distance = min_distance;
            furthest_point = i;
        }
    }

    return furthest_point;
}

float distance_from_segment(const pair<Point, Point>& segment, const Point& point)
{
    Point p1 = segment.first;
    Point p2 = segment.second;

    // float distance = abs(
    //                      (p2.x - p1.x) * (p1.y - point.y) - (p1.x - point.x) * (p2.y - p1.y))
    //     / sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));

    float distance = 0;

    pair<float, float> AB = { p2.x - p1.x, p2.y - p1.y };
    pair<float, float> BC = { point.x - p2.x, point.y - p2.y };
    pair<float, float> AC = { point.x - p1.x, point.y - p1.y };

    float AB_BC = AB.first * BC.first + AB.second * BC.second;
    float AB_AC = AB.first * AC.first + AB.second * AC.second;

    if (AB_BC > 0) {
        float y = point.y - p2.y;
        float x = point.x - p2.x;
        distance = sqrt(x * x + y * y);
    } else if (AB_AC < 0) {
        float y = point.y - p1.y;
        float x = point.x - p1.x;
        distance = sqrt(x * x + y * y);
    }else{
        //Find perpendicular distance to line
        float x1 = AB.first;
        float y1 = AB.second;
        float x2 = AC.first;
        float y2 = AC.second;
        float mod = sqrt(x1 * x1 + y1 * y1);
        distance = abs(x1 * y2 - y1 * x2) / mod;
    }

    return distance;
}

size_t find_closest_edge(const Point& point, const vector<pair<Point, Point>>& edges)
{
    float min_distance = FLT_MAX;
    size_t closest_edge = 0;

    for (int i = 0; i < edges.size(); i++) {

        float distance = distance_from_segment(edges[i], point);

        if (distance < min_distance) {
            min_distance = distance;
            closest_edge = i;
        }
    }

    return closest_edge;
}

std::vector<Point> edges_to_path(vector<pair<Point, Point>> edges){
    std::vector<Point> path;
    for(auto edge : edges){
        path.push_back(edge.first);
    }
    path.push_back(edges[0].first);
    return path;
}

int main(int argc, char** argv)
{
    vector<Point> not_in_path;
    vector<Point> in_path;

    vector<Point> opt_tour;
    ;
    load_tsp_file(argv[1], not_in_path);
    std::string path(argv[1]);
    auto opt_path = path.replace(path.find(".tsp"), 4, ".opt.tour");
    load_opt_tour(opt_path, not_in_path, opt_tour);

    vector<pair<Point, Point>> edges;

    auto calc_start = std::chrono::high_resolution_clock::now();

    auto starting_point_i = min_element(not_in_path.begin(), not_in_path.end(), point_sort_f);
    Point starting_point = *starting_point_i;
    in_path.push_back(starting_point);
    not_in_path.erase(starting_point_i);

    size_t furthest_index = find_furthest_index(in_path, not_in_path);
    Point second_point = not_in_path[furthest_index];
    in_path.push_back(second_point);
    not_in_path.erase(not_in_path.begin() + furthest_index);

    edges.push_back({ starting_point, second_point });
    edges.push_back({ second_point, starting_point });

    while (!not_in_path.empty()) {
        size_t furthest_index = find_furthest_index(in_path, not_in_path);
        Point furthest = not_in_path[furthest_index];
        not_in_path.erase(not_in_path.begin() + furthest_index);
        in_path.push_back(furthest);

        size_t closest_edge_index = find_closest_edge(furthest, edges);
        Point p1 = edges[closest_edge_index].first;
        Point p2 = edges[closest_edge_index].second;
        pair<Point, Point> new_edge_1 = { p1, furthest };
        pair<Point, Point> new_edge_2 = { furthest, p2 };
        //overwrites old edge
        edges[closest_edge_index] = new_edge_2;
        edges.insert(edges.begin() + closest_edge_index, new_edge_1);

        auto tour = edges_to_path(edges);
        print_tour(tour);
    }

    auto calc_stop = std::chrono::high_resolution_clock::now();

    auto tour = edges_to_path(edges);
    auto len = path_len(tour);
    auto len_opt = path_len(opt_tour);
    float gap = calc_gap(len, len_opt);
    std::cout << "Gap to optimal tour: " << gap << "\n";
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(calc_stop - calc_start).count();
    std::cout << "Time: " << time << " ms";
}
