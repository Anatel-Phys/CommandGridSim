#pragma once
#include <vector>
#include <string>
#include <fstream>

struct Point
{
	float x, y;

	Point();
	Point(float _x, float _y);
	//Point(int _x, float _y);
	//Point(int _x, int _y);
	//Point(float _x, int _y);
};

struct Coord
{
	int i, j;
};

float abs_mean_dist(Point a, Point b);

//first implementation : just an ordered list of points (to apply Dynamic Time Warping) => when grid is detecting multiple inputs, it will just compute the mean
struct Sketch
{
	std::vector<Point> points;
	size_t size;
	size_t classIdx;
};

Sketch process_sketch(std::vector<std::vector<Coord>*>& sketch, size_t width, size_t height);

bool save_sketch(Sketch s, std::string path);
bool save_sketches(std::vector<Sketch> sketches, std::string path);

Sketch load_sketch(std::string path);
std::vector<Sketch> load_sketches(std::string path);
