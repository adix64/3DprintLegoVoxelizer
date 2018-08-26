#pragma once

#include <include/glm.h>
#include <include/gl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <Core/GPU/Mesh.h>

class BaseMesh {
public:
	BaseMesh(unsigned int vbo, unsigned int ibo, unsigned int vao, unsigned int count);
	~BaseMesh();
	void draw(unsigned int topology = GL_LINES);
	void drawInstanced(unsigned int instances, unsigned int topology = GL_LINES);
//private:
	unsigned int vbo, ibo, vao, count;
};

typedef std::vector<VertexFormat> TVertexList;
typedef std::vector<uint32_t> TIndexList;

BaseMesh* generatePointMesh();
BaseMesh* generateLineMesh();