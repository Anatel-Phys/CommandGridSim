#include "DataModule.h"

Point::Point()
{
}

Point::Point(float _x, float _y)
{
	x = _x;
	y = _y;
}

//Point::Point(int _x, float _y)
//{
//	x = static_cast<float>(_x);
//	y = _y;
//}
//
//Point::Point(int _x, int _y)
//{
//	x = static_cast<float>(_x);
//	y = static_cast<float>(_y);
//}
//
//Point::Point(float _x, int _y)
//{
//	x = _x;
//	y = static_cast<float>(_y);
//}

float abs_mean_dist(Point a, Point b)
{
	return 1. / 2. * ((fabs(a.x - b.x)) + fabs(a.y - b.y));
}

Sketch process_sketch(std::vector<std::vector<Coord>*>& sketch, size_t width, size_t height)
{
	std::vector<Point> points;

	Point buffer;
	for (std::vector<Coord>* p_v : sketch)
	{
		if (p_v->size() > 0) //only appends if there is point at this step => concatenates every empty series
		{
			buffer.x = 0;
			buffer.y = 0;
			for (Coord c : *p_v)
			{
				buffer.x += c.i;
				buffer.y += c.j;
			}
			buffer.x /= p_v->size();
			buffer.y /= p_v->size();
			if (width > 1) //not optimal (way too much chekcs) but not a huge deal either
				buffer.x /= (width - 1);
			if (height > 1)
				buffer.y /= (height - 1);
			points.push_back(buffer);
		}
	}

	Sketch s;
	s.size = points.size();
	s.points = points;
	return s;
}

bool save_sketch(Sketch s, std::string path)
{
	std::ofstream f(path);

	if (f.is_open())
	{
		f << s.size << '\n';
		for (Point p : s.points)
		{
			f << p.x << " " << p.y << " ";
		}
		f.close();
		return true;
	}
	return false;
}

bool save_sketches(std::vector<Sketch> sketches, std::string path)
{
	std::ofstream f(path);

	if (f.is_open())
	{
		for (Sketch s : sketches)
		{
			f << s.size << "\n";
			for (Point p : s.points)
			{
				f << p.x << " " << p.y << " ";
			}
			f << "\n";
		}
		f.close();
		return true;
	}
	return false;
}

Sketch load_sketch(std::string path)
{
	Sketch s;

	std::ifstream f(path);
	if (f.is_open())
	{
		f >> s.size;

		std::vector<Point> v(s.size);
		for (size_t i = 0; i < s.size; i++)
		{
			f >> v.at(i).x;
			f >> v.at(i).y;
		}
		s.points = v;

		f.close();
		return s;
	}
	return Sketch();
}

std::vector<Sketch> load_sketches(std::string path)
{
	std::vector<Sketch> sketches;
	
	Sketch buffer;
	std::ifstream f(path);
	if (f.is_open())
	{
		while (f >> buffer.size)
		{
			std::vector<Point> v(buffer.size);
			for (size_t i = 0; i < buffer.size; i++)
			{
				f >> v.at(i).x;
				f >> v.at(i).y;
			}
			buffer.points = v;
			sketches.push_back(buffer);
		}

		f.close();
		return sketches;
	}
}
