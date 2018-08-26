#pragma once
#include "MeshVoxelizer.h"

int FurthestPointAlongDir(std::vector<glm::vec3> &positions, std::vector<unsigned int> &indices, glm::vec3 &direction) {
	uint farthestIDX = 0;
	if (positions.size() <= 0)
		return -1;
	GLfloat farthest = glm::dot(positions[indices[0]], direction);
	for (int it = 0; it < indices.size(); it++) {
		GLfloat dist = glm::dot(positions[indices[it]], direction);
		if (dist > farthest) {
			farthestIDX = indices[it];
			farthest = dist;
		}
	}
	return farthestIDX;
}

int FurthestPointAlongDir(std::unordered_map<int, glm::vec3> &verts, glm::vec3 &direction) {
	uint farthestIDX = verts.begin()->first;
	GLfloat farthest = glm::dot(verts[verts.begin()->first], direction);
	for (auto it = verts.begin(); it != verts.end(); it++) {
		GLfloat dist = glm::dot(it->second, direction);
		if (dist > farthest) {
			farthestIDX = it->first;
			farthest = dist;
		}
	}
	return farthestIDX;
}

int IDofClosestCentroidToPoint(std::unordered_map<int, glm::vec3> &centroids, glm::vec3 pPoint, float thresh)
{
	float minDist = 99999.f, dist;
	int bestID;
	for (auto it = centroids.begin(); it != centroids.end(); it++)
	{
		dist = glm::length(it->second - pPoint);
		if (dist < minDist)
		{
			minDist = dist;
			bestID = it->first;
		}
	}
	if (minDist < thresh)
		return bestID;
	else
		return -666;
}
