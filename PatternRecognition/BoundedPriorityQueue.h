#pragma once
#include <vector>
#include <iostream>

#define parent(i) (i/2)
#define left(i) (2*i + 1)
#define right(i) (2*i + 2)

template<class T>
struct Node
{
	float key;
	T* data;
};

template<class T>
inline void max_heapify(std::vector<Node<T>*>& nodes, size_t i, size_t heapSize)
{
	size_t l = left(i);
	size_t r = right(i);
	size_t largest;

	if (l < heapSize && nodes.at(l)->key > nodes.at(i)->key)
		largest = l;
	else
		largest = i;
	if (r < heapSize && nodes.at(r)->key > nodes.at(largest)->key)
		largest = r;
	if (largest != i)
	{
		Node<T>* buffer = nodes.at(largest);
		nodes.at(largest) = nodes.at(i);
		nodes.at(i) = buffer;
		max_heapify<T>(nodes, largest, heapSize);
	}
}

template<class T>
class BoundedPriorityQueue
{
private:
	std::vector<Node<T>*> nodes;
	size_t capacity;
	size_t size;

public:
	BoundedPriorityQueue(size_t _capacity)
	{
		capacity = _capacity;
		size = 0;
		nodes.resize(capacity);
	};

	size_t get_capacity() { return capacity; }
	size_t get_size() { return size; }
	float get_max_key() { return nodes.at(0)->key; }
	bool insert(T* _data, float _key)
	{
		if (size < capacity)
		{
			Node<T>* n = new Node<T>;
			n->data = _data;
			n->key = _key;

			nodes.at(size) = n;

			size_t i = size;
			Node<T>* buffer;
			while (nodes.at(i)->key > nodes.at(parent(i))->key)
			{
				buffer = nodes.at(parent(i));
				nodes.at(parent(i)) = nodes.at(i);
				nodes.at(i) = buffer;
				i = parent(i);
			}

			size++;
			return true;
		}

		if (_key < get_max_key())
		{
			nodes.at(0)->key = _key;
			nodes.at(0)->data = _data;
			max_heapify(nodes, 0, size);
			return true;
		}
		return false;
	}


	//debug
	void print_queue()
	{
		std::cout << "Size : " << size << std::endl << "Keys :\n";
		for (size_t i = 0; i < size; i++)
		{
			std::cout << nodes.at(i)->key << " ";
		}
		std::cout << std::endl;
	}

	std::vector<T*> get_items()
	{
		std::vector<T*> items(size);
		for (size_t i = 0; i < size; i++)
			items.at(i) = nodes.at(i)->data;
		return items;
	}
};
