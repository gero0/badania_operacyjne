#pragma once

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

struct Point {
    unsigned int id;
    float x;
    float y;
};

int dist(Point p1, Point p2)
{
    int xd = p2.x - p1.x;
    int yd = p2.y - p1.y;
    return (int)(0.5f + std::sqrt(xd * xd + yd * yd));
}

unsigned int path_len(const std::vector<Point>& path)
{
    unsigned int len = 0;
    for (int i = 0; i < path.size() - 1; i++) {
        len += dist(path[i], path[i + 1]);
    }
    return len;
}

bool point_sort_f(Point p1, Point p2)
{
    if (p1.x == p2.x)
        return p1.y < p2.y;

    return p1.x < p2.x;
}

void skiplines(std::fstream& file, int N)
{
    for (int currLineNumber = 0; currLineNumber < N; ++currLineNumber) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), file.widen('\n'));
        //just skipping the line
    }
}

void load_tsp_file(std::string path, std::vector<Point>& points)
{
    std::fstream file;
    file.open(path);

    skiplines(file, 3);

    std::string temp;
    int N;
    file >> temp;
    if (temp == "DIMENSION:")
        file >> N;
    else
        file >> temp >> N;

    skiplines(file, 3);

    for (unsigned int i = 0; i < N; i++) {
        unsigned int id;
        float x, y;
        file >> id >> x >> y;
        points.push_back({ id - 1, x, y });
    }
    file.close();
}

void load_opt_tour(std::string path, const std::vector<Point>& points, std::vector<Point>& opt_tour)
{
    std::fstream file;
    file.open(path);

    std::string temp("");
    int N;
    while(temp != "DIMENSION" && temp != "DIMENSION:"){
        file >> temp;
    }
    if (temp == "DIMENSION:")
        file >> N;
    else
        file >> temp >> N;

    skiplines(file, 2);

    for (unsigned int i = 0; i < N; i++) {
        unsigned int id;
        // std::string id;
        file >> id;
        Point point;
        for (auto p : points) {
            if (p.id == id - 1) {
                point = p;
            }
        }
        opt_tour.push_back(point);
    }
    file.close();
}

std::vector<Point> alg_123(std::vector<Point>& points)
{
    std::vector<Point> tour;
    for (int i = 0; i < points.size(); i++) {
        tour.push_back(points[i]);
    }
    tour.push_back(points[0]);
    return tour;
}

void print_tour(std::vector<Point> tour)
{
    std::cout << "PATH";
    for (auto point : tour) {
        std::cout << " " << point.id + 1;
    }
    std::cout << "\n";
}

float calc_gap(int current_len, int opt_len)
{
    int gap = current_len - opt_len;
    float percent = ((float)gap / opt_len) * 100.0f;
    return percent;
}

class Logger {
public:
    Logger(std::string path)
    {
        file.open(path, std::ios::out);
        std::string header("Iteration;current;opt;gap\n");
        file.write(header.c_str(), header.length());
    }

    void log(unsigned int it_n, int current_len, int opt_len, float gap){
        std::stringstream ss;
        ss << it_n << ';' << current_len << ';' << opt_len << ';' << gap << '\n';
        file.write(ss.str().c_str(), ss.str().length());
    }

    ~Logger() {
        file.close();
    }

private:
    std::fstream file;
};
