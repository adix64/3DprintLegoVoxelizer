#pragma once
#include "BaseMesh.hpp"

BaseMesh::BaseMesh(unsigned int vbo, unsigned int ibo, unsigned int vao, unsigned int count) {
	this->vbo = vbo;
	this->ibo = ibo;
	this->vao = vao;
	this->count = count;
}
BaseMesh::~BaseMesh() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
}
void BaseMesh::draw(unsigned int topology) {
	glBindVertexArray(vao);
	glDrawElements(topology, count, GL_UNSIGNED_INT, (void*)0);
}
void BaseMesh::drawInstanced(unsigned int instances, unsigned int topology) {
	glBindVertexArray(vao);
	glDrawElementsInstanced(topology, count, GL_UNSIGNED_INT, (void*)0, instances);
}

typedef std::vector<VertexFormat> TVertexList;
typedef std::vector<uint32_t> TIndexList;

BaseMesh* generatePointMesh()
{
	VertexFormat point;

	TVertexList verts;
	verts.push_back(VertexFormat(0, 0, 0));
	TIndexList indices; indices.push_back(0);
	//obiecte OpenGL mesh
	unsigned int vbo, ibo, vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*verts.size(), &verts[0], GL_STATIC_DRAW);
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0])*indices.size(), &indices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
	return new BaseMesh(vbo, ibo, vao, 1);//????
}


BaseMesh* generateLineMesh()
{
	std::vector<VertexFormat> vertices;
	vertices.push_back(VertexFormat(-40, -25, 10));
	vertices.push_back(VertexFormat(-40, 55, 10));
	std::vector<unsigned int> indices;
	indices.push_back(0);
	indices.push_back(1);

	unsigned int vbo, ibo, vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
	return new BaseMesh(vbo, ibo, vao, indices.size());
}