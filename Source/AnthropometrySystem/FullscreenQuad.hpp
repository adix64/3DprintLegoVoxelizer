#pragma once
#include <include/glm.h>
#include <include/gl.h>
#include <Core\GPU\BaseMesh.hpp>

class FullScreenQuad
{
public:
	FullScreenQuad()
	{
		fsQuadMesh = generateFSQuad();
	}
	~FullScreenQuad()
	{
		delete fsQuadMesh;
	}

	void Init(Shader *shader, int *w, int *h)
	{
		shaderHandle = shader;
		width = w;
		height = h;
	}
	void Draw(unsigned int tex)
	{
		glDisable(GL_DEPTH_TEST);
		glUseProgram(shaderHandle->GetProgramID());
		glViewport(0, 0, *width, *height);
		//glClearColor(0.0f, 0.0f, 0.0f, 1);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0 + 1);

		glBindTexture(GL_TEXTURE_2D, tex);// colorPickingFB.getColorTexture());

		glUniform1i(glGetUniformLocation(shaderHandle->GetProgramID(), "fullscreenTex"), 1);
		glUniform1i(glGetUniformLocation(shaderHandle->GetProgramID(), "width"), *width);
		glUniform1i(glGetUniformLocation(shaderHandle->GetProgramID(), "height"), *height);

		fsQuadMesh->draw(GL_TRIANGLES);
	}
private:
BaseMesh* generateFSQuad()
{
	TVertexList verts;
	verts.push_back(VertexFormat(-1, -1, 0, 0, 0));
	verts.push_back(VertexFormat(1, -1, 0, 1, 0));
	verts.push_back(VertexFormat(1, 1, 0, 1, 1));
	verts.push_back(VertexFormat(-1, 1, 0, 0, 1));


	TIndexList indices;

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(2);


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
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(3 * sizeof(GLfloat)));
	return new BaseMesh(vbo, ibo, vao, indices.size());
}
private:
	BaseMesh *fsQuadMesh;
	Shader *shaderHandle;
	int *width, *height;
};