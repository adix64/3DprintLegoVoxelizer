#pragma once
#include <include/glm.h>
#include <Core\GPU\BaseMesh.hpp>
#include <Core\GPU\Shader.h>

class Grid
{
public:
	Grid() {}
	~Grid() { delete gridMesh; }
	void Init(Shader *shader, glm::mat4 *view, glm::mat4 *proj)
	{
		shaderHandle = shader;
		gridMesh = generateGrid(14, 5.0f);
		view_matrix = view;
		projection_matrix = proj;
	}

	BaseMesh* generateGrid(uint8_t numCells, float cellSize)
	{
		TVertexList gridVerts;
		TIndexList gridIndices;
		float extremity = (numCells + 1) / 2 * cellSize;
		for (float row = -extremity; row < extremity; row += cellSize)
		{
			VertexFormat vH1(-extremity, 0, row);
			VertexFormat vH2(extremity - cellSize, 0, row);
//			printf("vH1: %f %f     vH2: %f %f\n", vH1.position.x, vH1.position.y, vH2.position.x, vH2.positiony);
			gridVerts.push_back(vH1); gridVerts.push_back(vH2);

			VertexFormat vV1(row, 0, -extremity);
			VertexFormat vV2(row, 0, extremity - cellSize);
			gridVerts.push_back(vV1); gridVerts.push_back(vV2);

			uint32_t sz = gridVerts.size();
			gridIndices.push_back(sz - 4);
			gridIndices.push_back(sz - 3);
			gridIndices.push_back(sz - 2);
			gridIndices.push_back(sz - 1);
		}
		//obiecte OpenGL mesh
		unsigned int vbo, ibo, vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat)*gridVerts.size(), &gridVerts[0], GL_STATIC_DRAW);
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gridIndices[0])*gridIndices.size(), &gridIndices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
		return new BaseMesh(vbo, ibo, vao, gridIndices.size());
	}
	void DrawGrid(glm::mat4 &model_matrix, glm::vec3 color = glm::vec3(0.1, 0.15, 0.25))
	{
		//foloseste shaderul
		glUseProgram(shaderHandle->GetProgramID());
		glLineWidth(1);
		//trimite variabile uniforme la shader
		glUniformMatrix4fv(glGetUniformLocation(shaderHandle->GetProgramID(), "Model"), 1, false, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(glGetUniformLocation(shaderHandle->GetProgramID(), "View"), 1, false, glm::value_ptr(*view_matrix));
		glUniformMatrix4fv(glGetUniformLocation(shaderHandle->GetProgramID(), "Projection"), 1, false, glm::value_ptr(*projection_matrix));
		glUniform3f(glGetUniformLocation(shaderHandle->GetProgramID(), "color"), color.x,color.y,color.z);
		//gridMesh->draw(); //XoY
		glUniformMatrix4fv(glGetUniformLocation(shaderHandle->GetProgramID(), "Model"), 1, false, glm::value_ptr(model_matrix));
		gridMesh->draw(); //XoZ
		//model_matrix = glm::rotate(glm::mat4(1), glm::radians(90.f), glm::vec3(0, 1, 0));
		//glUniformMatrix4fv(glGetUniformLocation(shaderHandle->GetProgramID(), "model_matrix"), 1, false, glm::value_ptr(model_matrix));
		//gridMesh->draw(); //YoZ
	}
private:
	Shader *shaderHandle;
	BaseMesh *gridMesh;
	glm::mat4 *view_matrix, *projection_matrix;
};