#pragma once

#include <cmath>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include <iostream>

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
    if(temp == "DIMENSION:")
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

std::vector<Point> alg_123(std::vector<Point>& points)
{
    std::vector<Point> tour;
    for (int i = 0; i < points.size(); i++) {
        tour.push_back(points[i]);
    }
    tour.push_back(points[0]);
    return tour;
}

void print_tour(std::vector<Point> tour){
    std::cout << "PATH";
    for(auto point : tour){
        std::cout << " " << point.id + 1;
    }
    std::cout << "\n";
}