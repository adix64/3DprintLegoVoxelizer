#pragma once
#include <include/glm.h>
#include <Core\GPU\BaseMesh.hpp>
#include <vector>
#include <Core\GPU\Shader.h>
#include "Camera.hpp"
class Gizmo
{
public:
	enum GizmoMode { MOVE_MODE, ROTATE_MODE };
	GizmoMode crtMode = MOVE_MODE;

	Gizmo() {}
	~Gizmo()
	{
	}
	void Init(Shader *shader, glm::mat4 *view, glm::mat4 *proj)
	{
		m_shaderHandle = shader;
		projection_matrix = proj;
		view_matrix = view;
		selectedAxisX = selectedAxisY = selectedAxisZ = false;
		gizmoCone = generateGizmoCone();
		gizmoLine = generateGizmoLine();
		gizmoCircle = generateGizmoCircle();
	}

	void setSelectedX(bool val) { selectedAxisX = val; }
	void setSelectedY(bool val) { selectedAxisY = val; }
	void setSelectedZ(bool val) { selectedAxisZ = val; }

	bool getSelectedX() { return selectedAxisX; }
	bool getSelectedY() { return selectedAxisY; }
	bool getSelectedZ() { return selectedAxisZ; }

	void Render(Camera &camera, glm::vec3 pos)
	{
		if (!isVisible)
			return;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUseProgram(m_shaderHandle->GetProgramID());

		float scalefact = glm::distance(camera.GetPosition(), pos) * 0.025;
		glm::mat4 scalemat = glm::scale(glm::mat4(1), glm::vec3(scalefact, scalefact, scalefact));
										  //trimite variabile uniforme la shader

		glUniformMatrix4fv(glGetUniformLocation(m_shaderHandle->GetProgramID(), "View"), 1, false, glm::value_ptr(*view_matrix));
		glUniformMatrix4fv(glGetUniformLocation(m_shaderHandle->GetProgramID(), "Projection"), 1, false, glm::value_ptr(*projection_matrix));

		glm::mat4 translateM = glm::translate(glm::mat4(1), pos);
		model_matrix = translateM * scalemat;
		glUniformMatrix4fv(glGetUniformLocation(m_shaderHandle->GetProgramID(), "Model"), 1, false, glm::value_ptr(model_matrix));
		glUniform3f(glGetUniformLocation(m_shaderHandle->GetProgramID(), "color"), 0, 1., 0);
		
		if(crtMode == MOVE_MODE)
		{
			if (selectedAxisY)
				glUniform3f(glGetUniformLocation(m_shaderHandle->GetProgramID(), "color"), 1., 1., 0);
			gizmoLine->draw(GL_LINES);
			gizmoCone->draw(GL_TRIANGLES);

			model_matrix = translateM * glm::rotate(glm::mat4(1), glm::radians(90.f), glm::vec3(1, 0, 0))* scalemat;
			glUniformMatrix4fv(glGetUniformLocation(m_shaderHandle->GetProgramID(), "Model"), 1, false, glm::value_ptr(model_matrix));
			glUniform3f(glGetUniformLocation(m_shaderHandle->GetProgramID(), "color"), 0, 0., 1);
			if (selectedAxisZ)
				glUniform3f(glGetUniformLocation(m_shaderHandle->GetProgramID(), "color"), 1., 1., 0);

			gizmoLine->draw(GL_LINES);
			gizmoCone->draw(GL_TRIANGLES);

			model_matrix = translateM * glm::rotate(glm::mat4(1), glm::radians(90.f), glm::vec3(0, 0, 1))* scalemat;
			glUniformMatrix4fv(glGetUniformLocation(m_shaderHandle->GetProgramID(), "Model"), 1, false, glm::value_ptr(model_matrix));
			glUniform3f(glGetUniformLocation(m_shaderHandle->GetProgramID(), "color"), 1, 0., 0);
			if (selectedAxisX)
				glUniform3f(glGetUniformLocation(m_shaderHandle->GetProgramID(), "color"), 1., 1., 0);

			gizmoLine->draw(GL_LINES);
			gizmoCone->draw(GL_TRIANGLES);
	
		}
		else 
		{
			if (selectedAxisY)
				glUniform3f(glGetUniformLocation(m_shaderHandle->GetProgramID(), "color"), 1., 1., 0);
			gizmoCircle->draw(GL_LINES);

			model_matrix = translateM * glm::rotate(glm::mat4(1), glm::radians(90.f), glm::vec3(1, 0, 0))* scalemat;
			glUniformMatrix4fv(glGetUniformLocation(m_shaderHandle->GetProgramID(), "Model"), 1, false, glm::value_ptr(model_matrix));
			glUniform3f(glGetUniformLocation(m_shaderHandle->GetProgramID(), "color"), 0, 0., 1);
			if (selectedAxisZ)
				glUniform3f(glGetUniformLocation(m_shaderHandle->GetProgramID(), "color"), 1., 1., 0);

			gizmoCircle->draw(GL_LINES);

			model_matrix = translateM * glm::rotate(glm::mat4(1), glm::radians(90.f), glm::vec3(0, 0, 1))* scalemat;
			glUniformMatrix4fv(glGetUniformLocation(m_shaderHandle->GetProgramID(), "Model"), 1, false, glm::value_ptr(model_matrix));
			glUniform3f(glGetUniformLocation(m_shaderHandle->GetProgramID(), "color"), 1, 0., 0);
			if (selectedAxisX)
				glUniform3f(glGetUniformLocation(m_shaderHandle->GetProgramID(), "color"), 1., 1., 0);

			gizmoCircle->draw(GL_LINES);
		}
	}
	void SetVisible(bool b) { isVisible = b; }
private:
	BaseMesh* generateGizmoLine()
	{
		VertexFormat point;
		TVertexList verts;
		verts.push_back(VertexFormat(0, 8, 0));
		verts.push_back(VertexFormat(0, 0, 0));
		TIndexList indices; indices.push_back(0);
		indices.push_back(1);
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
		return new BaseMesh(vbo, ibo, vao, 2);//????
	}
	BaseMesh* generateGizmoCone()
	{
		TVertexList coneVerts;
		TIndexList coneIndices;
		coneVerts.push_back(VertexFormat(0, 9, 0));

		int idx = 1;
		const float step = 360 / 40.0f;
		for (float angle = 0; angle <= 360; angle += 2 * step)
		{
			glm::vec3 vcrt = glm::vec3(glm::rotate(glm::mat4(1), glm::radians(angle), glm::vec3(0, 1, 0)) * glm::vec4(0.5, 7.59, 0, 1));
			glm::vec3 vnext = glm::vec3(glm::rotate(glm::mat4(1), glm::radians(angle + step), glm::vec3(0, 1, 0)) * glm::vec4(0.5, 7.59, 0, 1));
			coneVerts.push_back(VertexFormat(vcrt.x, vcrt.y, vcrt.z));
			coneVerts.push_back(VertexFormat(vnext.x, vnext.y, vnext.z));
			coneIndices.push_back(coneVerts.size() - 1);
			coneIndices.push_back(coneVerts.size() - 2);
			coneIndices.push_back(0);
			coneIndices.push_back(coneVerts.size() - 2);
			coneIndices.push_back(coneVerts.size() - 3);
			coneIndices.push_back(0);

			idx += 2;
		}

		unsigned int vao, vbo, ibo, count = coneIndices.size();
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * coneVerts.size(), &coneVerts[0], GL_STATIC_DRAW);
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(coneIndices[0])*coneIndices.size(), &coneIndices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
		return new BaseMesh(vbo, ibo, vao, count);
	}
	BaseMesh* generateGizmoCircle()
	{
		TVertexList circleVerts;
		TIndexList circleIndices;
	
		const float step = 360 / 40.0f;
		for (float angle = 0; angle <= 360; angle += step)
		{
			glm::vec3 vcrt = glm::vec3(glm::rotate(glm::mat4(1), glm::radians(angle), glm::vec3(0, 1, 0)) * glm::vec4(3, 0, 0, 1));
			glm::vec3 vnext = glm::vec3(glm::rotate(glm::mat4(1), glm::radians(angle + step), glm::vec3(0, 1, 0)) * glm::vec4(3, 0, 0, 1));
			circleVerts.push_back(VertexFormat(vcrt.x, vcrt.y, vcrt.z));
			circleVerts.push_back(VertexFormat(vnext.x, vnext.y, vnext.z));
			circleIndices.push_back(circleVerts.size() - 1);
			circleIndices.push_back(circleVerts.size() - 2);
		}

		unsigned int vao, vbo, ibo, count = circleIndices.size();
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * circleVerts.size(), &circleVerts[0], GL_STATIC_DRAW);
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circleIndices[0])*circleIndices.size(), &circleIndices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
		return new BaseMesh(vbo, ibo, vao, count);
	}
private:
	Shader *m_shaderHandle;
	bool selectedAxisX, selectedAxisY, selectedAxisZ;
	BaseMesh *gizmoLine, *gizmoCone, *gizmoCircle;
	glm::mat4 model_matrix;
	glm::mat4 *view_matrix, *projection_matrix;
	bool isVisible = false;
};