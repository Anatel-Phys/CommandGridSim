#include "DynamicTimeWarping.h"



//distances axis x is s1, y is s2
double DTW_distance(Sketch& s1, Sketch& s2, float max_dist)
{
	std::vector<float>* distances = new std::vector<float>(s1.size * s2.size);

	distances->at(0) = abs_mean_dist(s1.points.at(0), s2.points.at(0));

	for (size_t i = 1; i < s1.size; i++)
		distances->at(i) = distances->at(i - 1) + abs_mean_dist(s1.points.at(i), s2.points.at(0));
	for (size_t j = 1; j < s2.size; j++)
		distances->at(j * s1.size) = distances->at((j - 1) * s1.size) + abs_mean_dist(s1.points.at(0), s2.points.at(j));

	float d, rowMinDistance; //rowMinDistances keeps track of the minimal distance computed in a given row. If it is greater than max_dist, then these sketches are too far apart ->not neighbors
	for (size_t j = 1; j < s2.size; j++)
	{
		rowMinDistance = distances->at(j * s1.size);
		for (size_t i = 1; i < s1.size; i++)
		{
			d = distances->at(j * s1.size + i - 1);
			if (distances->at((j - 1) * s1.size + i) < d)
				d = distances->at((j - 1) * s1.size + i);
			if (distances->at((j - 1) * s1.size + i - 1) < d)
				d = distances->at((j - 1) * s1.size + i - 1);
			d += abs_mean_dist(s1.points.at(i), s2.points.at(j));
			
			if (d < rowMinDistance)
				rowMinDistance = d;

			distances->at(j * s1.size + i) = d;
		}

		if (rowMinDistance > max_dist)
		{
			delete distances;
			return FLOAT_MAX;
		}
	}
	d = distances->at(s1.size * s2.size - 1);
	delete distances;
	return d;
}

size_t knn_class(Sketch query, std::vector<Sketch> dataset, size_t k)
{
	BoundedPriorityQueue<Sketch> bpq(k);

	float d = DTW_distance(query, dataset.at(0), FLOAT_MAX);
	bpq.insert(&dataset.at(0), d);

	for (size_t i = 1; i < dataset.size(); i++)
	{
		d = DTW_distance(query, dataset.at(i), bpq.get_max_key());
		bpq.insert(&dataset.at(i), d);
	}

	std::vector<Sketch*> items = bpq.get_items();
	int classes[CLASS_MAX];
	for (size_t i = 0; i < bpq.get_size(); i++)
	{
		classes[items.at(i)->classIdx]++;
	}
	int maxClass = 0;
	for (size_t i = 0; i < CLASS_MAX; i++)
	{
		if (classes[i] > classes[maxClass])
			maxClass = i;
	}

	return maxClass;
}
