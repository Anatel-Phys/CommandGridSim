#include "SketchInteraction.h"
#include "DynamicTimeWarping.h"
#include "PatternSimulation.h"
#include <cmath>

static int nSensorX = 11;
static int nSensorY = 11;

float lerp(float x1, float x2, float y1, float y2, float x) //should return middle of interval if x is too far from x1 of x2 bcs else it'll assume an extrapolation that would imply that a sensor has been crossed ->"rectified lerp"
{
	float val = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
	//if (val <  std::min(y1, y2) - 1)
	//	return std::min(y1, y2) - 1;
	//if (val > std::max(y1, y2) + 1)
	//	return std::max(y1, y2) + 1;

	if (x > x2)
	{
		if (y2 > y1)
		{
			if (val > y2 + 0.5f)
			{
				return y2 + 0.5f;
			}
		}
		else
		{
			if (val < y2 - 0.5f)
				return y2 - 0.5f;
		}
	}
	else if (x < x1)
	{
		if (y1 > y2)
		{
			if (val > y1 + 0.5f)
			{
				return y1 + 0.5f;
			}
		}
		else
		{
			if (val < y1 - 0.5f)
			{
				return y1 - 0.5f;
			}
		}
	}
	return val;
}

struct TimeSerie
{
	std::vector<TimePoint> x, y;
	size_t xSize, ySize;

	TimeSerie(std::vector<TimePoint> i_x, std::vector<TimePoint> i_y)
	{
		xSize = i_x.size();
		ySize = i_y.size();

		x = i_x;
		y = i_y;
	}
};

struct Pattern
{
	std::vector<Point> points;
	size_t size;
	int classIdx;

	Pattern(std::vector<Point> i_points, int i_classIdx = 0)
	{
		points = i_points;
		size = i_points.size();
		classIdx = i_classIdx;
	}
};

float interpolate(std::vector<TimePoint>& v, float t)
{
	int i;
	if (t < v.at(0).t)
		i = 0;
	else if (t > v.at(v.size() - 1).t)
		i = v.size() - 2; //bcs i and i + 1 will be used anyway
	else
	{
		int a = 0;
		int b = v.size() - 1;
		int mid;

		while (b - a > 1)
		{
			mid = (a + b) / 2;

			if (v.at(mid).t > t)
				b = mid;
			else
				a = mid;
		}
		i = a;
	}

	return lerp(v.at(i).t, v.at(i + 1).t, v.at(i).val, v.at(i + 1).val, t);
}

Pattern stupid_algorithm(TimeSerie ts)
{
	if (ts.xSize <= 1 || ts.ySize <= 1)
	{
		std::cout << "Pattern almost empty, returning empty pattern" << std::endl;
		return Pattern(std::vector<Point>());
	}

	std::vector<Point> points;

	float curTime;
	for (int i = 0; i < ts.xSize; i++)
	{
		curTime = ts.x.at(i).t;
		points.emplace_back(ts.x.at(i).val, interpolate(ts.y, curTime));
	}

	for (int i = 0; i < ts.ySize; i++)
	{
		curTime = ts.y.at(i).t;
		points.emplace_back(interpolate(ts.x, curTime), ts.y.at(i).val);
	}

	return Pattern(points);
}

Pattern complicated_algorithm(TimeSerie ts)
{
	float buf;
	std::vector<Point> points;

	if (ts.xSize == 0)
	{
		if (ts.ySize == 0)
			return Pattern(std::vector<Point>());
		else
		{
			buf = static_cast<float>(nSensorX - 1) / 2.;
			for (int i = 0; i < ts.ySize; i++)
			{
				points.emplace_back(buf, ts.y.at(i).val);
			}
			return Pattern(points);
		}
	}
	else if (ts.ySize == 0)
	{
		buf = static_cast<float>(nSensorY - 1) / 2.;
		for (int i = 0; i < ts.xSize; i++)
		{
			points.emplace_back(ts.x.at(i).val, buf);
		}
		return Pattern(points);
	}
	else
	{
		int i = 0;
		int j = 0;

		if (ts.xSize == 1) //copies the value to have two points so the algorithm can lerp
		{
			ts.x.push_back(TimePoint(ts.x.at(0)));
			ts.x.at(1).t += 1.f;
			ts.xSize++;
		}
		if (ts.ySize == 1)
		{
			ts.y.push_back(TimePoint(ts.y.at(0)));
			ts.y.at(1).t += 1;
			ts.ySize++;
		}

		if (ts.x.at(0).t <= ts.y.at(0).t) //val extrapolation step
		{
			while (i < ts.xSize && ts.x.at(i).t < ts.y.at(0).t) 
			{
				//std::cout << "i : " << i << " " << ts.xSize << std::endl;
				points.emplace_back(ts.x.at(i).val, lerp(ts.y.at(0).t, ts.y.at(1).t, ts.y.at(0).val, ts.y.at(1).val, ts.x.at(i).t));
				i++;
			}
		}
		else
		{
			while (j < ts.ySize && ts.y.at(j).t <= ts.x.at(0).t)
			{
				//std::cout << "j : " << j << " " << ts.ySize << std::endl;
				points.emplace_back(lerp(ts.x.at(0).t, ts.x.at(1).t, ts.x.at(0).val, ts.x.at(1).val, ts.y.at(j).t), ts.y.at(j).val);
				j++;
			}
		}

		while (i < ts.xSize && j < ts.ySize) //main loop
		{
			while (j < ts.ySize && i < ts.xSize && ts.x.at(i).t <= ts.y.at(j).t) //if x[i].t < y[j].t AND i < xSize, after first step, it is guarenteed that y[j - 1] exists
			{
				//std::cout << "1:\nj : " << j << " " << ts.ySize << std::endl;
				//std::cout << "i : " << i << " " << ts.xSize << std::endl;
				points.emplace_back(ts.x.at(i).val, lerp(ts.y.at(j - 1).t, ts.y.at(j).t, ts.y.at(j - 1).val, ts.y.at(j).val, ts.x.at(i).t));
				i++;
			}

			while (i < ts.xSize && j < ts.ySize && ts.y.at(j).t <= ts.x.at(i).t)
			{
				//std::cout << "2:\nj : " << j << " " << ts.ySize << std::endl;
				//std::cout << "i : " << i << " " << ts.xSize << std::endl;
				points.emplace_back(lerp(ts.x.at(i - 1).t, ts.x.at(i).t, ts.x.at(i - 1).val, ts.x.at(i).val, ts.y.at(j).t), ts.y.at(j).val);
				j++;
			}
		}

		if (i < ts.xSize) //if not needed, actually. I'll let them here for clarity, for now
		{
			while (i < ts.xSize)
			{
				points.emplace_back(ts.x.at(i).val, lerp(ts.y.at(ts.ySize - 2).t, ts.y.at(ts.ySize - 1).t,
					ts.y.at(ts.ySize - 2).val, ts.y.at(ts.ySize - 1).val, ts.x.at(i).t));
				i++;
			}
		}
		else if (j < ts.ySize)
		{
			while (j < ts.ySize)
			{
				points.emplace_back(lerp(ts.x.at(ts.xSize - 2).t, ts.x.at(ts.xSize - 1).t,
					ts.x.at(ts.xSize - 2).val, ts.x.at(ts.xSize - 1).val, ts.y.at(j).t), ts.y.at(j).val);
				j++;
			}
		}
	}

	return Pattern(points);
}

std::vector<TimeSerie> split_patterns(std::vector<TimePoint> x, std::vector<TimePoint> y)
{
	int i = 0;
	int j = 0;

	std::vector<TimeSerie> series;
	std::vector<TimePoint> x_buf, y_buf;
	while (i < x.size() || j < y.size())
	{
		while (x.at(i).val != -1) //pour etre plus rapide pour l'implémentation finale : d'abord réserver l'espace en scannant les -1
		{
			x_buf.push_back(x.at(i));
			i++;
		}
		while (y.at(j).val != -1) //pour etre plus rapide pour l'implémentation finale : d'abord réserver l'espace en scannant les -1
		{
			y_buf.push_back(y.at(j));
			j++;
		}
		if (x_buf.size() > 0)
		{
			if (y_buf.size() > 0)
			{
				float min = std::min(x_buf.at(0).t, y_buf.at(0).t);
				std::cout << "Before : \n";
				std::cout << x_buf.at(0).t << std::endl;
				std::cout << y_buf.at(0).t << std::endl;
				x_buf.at(0).t -= min;
				y_buf.at(0).t -= min;
				std::cout << "After : \n";
				std::cout << x_buf.at(0).t << std::endl;
				std::cout << y_buf.at(0).t << std::endl;
			}
			else
				x_buf.at(0).t = 0.f;
		}
		else if (y_buf.size() > 0)
			y_buf.at(0).t = 0.f;

		series.emplace_back(x_buf, y_buf);
		i++;
		j++;

		x_buf.clear();
		y_buf.clear();
	}

	return series;
}

//pattern acquisition test for testing the interpolation algorithm
int main()
{
	PatternSimulation sim(nSensorX, nSensorY, 2., 0.02, 1000, 1000);

	while (sim.is_running())
		sim.run();

	auto sens_x = sim.get_patternX();
	auto sens_y = sim.get_patternY();

	std::vector<TimeSerie> series = split_patterns(sens_x, sens_y);

	TimeSerie ts = series.at(0);

	Pattern pat = complicated_algorithm(ts);

	std::ofstream f("data.txt", std::ios::trunc);
	for (Point p : pat.points)
	{
		f << p.x << "\t" << p.y << std::endl;
	}
	f.close();

	f = std::ofstream("x.txt", std::ios::trunc);
	for (TimePoint tp : ts.x)
	{
		f << tp.t << "\t" << tp.val << std::endl;
	}
	f.close();

	f = std::ofstream("y.txt", std::ios::trunc);
	for (TimePoint tp : ts.y)
	{
		f << tp.t << "\t" << tp.val << std::endl;
	}

}

//pattern design for testing the recognition algorithn
//int main()
//{
//
//	std::vector<Sketch> sketches(4);
//	for (size_t i = 0; i < sketches.size(); i++)
//	{
//		SketchCreator sc(11, 11);
//		sc.run();
//		sketches.at(i) = sc.get_processed_sketch();
//	}
//
//	sketches.at(0).classIdx = 0;
//	sketches.at(1).classIdx = 0;
//	sketches.at(2).classIdx = 1;
//	sketches.at(3).classIdx = 1;
//
//	SketchCreator sc(10, 10);
//	sc.run();
//	Sketch query = sc.get_processed_sketch();
//
//	std::cout << knn_class(query, sketches, 2);
//}