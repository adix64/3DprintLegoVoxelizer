#pragma once
#include <stdlib.h>

class DisjointSet
{
public:
	int N;
	std::vector<int> parent;// , processed;
	
	DisjointSet(int N){
		this->N = N;
		parent = std::vector<int>(N + 1, 0);
		//processed = std::vector<int>(N + 1, 0);
	}

	void Union(int x, int y) {
		if (x <= 0 || x > N || y <= 0 || y > N)
			return;
		//processed[x] = processed[y] = 1;
		int rx = Find(x), ry = Find(y);
		if (rx != ry) {
			parent[rx] = ry;
		}
	}

	int Find(int x) {
		if (x <= 0 || x > N)
			return 0;
		//if (!processed[x])
			//return  -1;
		while (parent[x] > 0)
			x = parent[x];
		return x;
	}
};

