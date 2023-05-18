#pragma once
#include "BoundedPriorityQueue.h"
#include "DataModule.h"
#include <limits>

#define FLOAT_MAX std::numeric_limits<float>::max()
#define CLASS_MAX 2

double DTW_distance(Sketch& s1, Sketch& s2, float max_dist);

size_t knn_class(Sketch query, std::vector<Sketch> dataset, size_t k);


