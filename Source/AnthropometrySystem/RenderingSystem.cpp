#include "MeshVoxelizer.h"
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include "DisjointSets.hpp"
#include <Core/Engine.h>
#include <algorithm>

#include <chrono>


glm::vec3 backgroundColors[6] = { glm::vec3(0.5), glm::vec3(0.33), glm::vec3(0), glm::vec3(0.105f, 0.11f, .4f), glm::vec3(1),
								  glm::vec3(0.66)};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MeshVoxelizer::LoadShaders()
{

	{// DULL COLOR
		Shader *shader = new Shader("FlatColor");
		shader->AddShader("Shaders/pointVertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Shaders/FlatColorFragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
	{// FULL-SCREEN SHADER
		Shader *shader = new Shader("FullScreenShader");
		shader->AddShader("Shaders/fullscreenVertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Shaders/fullscreenFragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{// DULL COLOR
		Shader *shader = new Shader("Text");
		shader->AddShader("Shaders/textVertex.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Shaders/textFragment.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//DEFAULT LIT GREY SHADER(hardcoded DirLight in shader)
	{
		Shader *shader = new Shader("default");
		shader->AddShader("Shaders/defaultVertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Shaders/defaultFragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//FLAT COLOR SHADER
	{
		Shader *shader = new Shader("flat");
		shader->AddShader("Shaders/defaultVertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Assets/Shaders/Color.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//VERTEX COLOR SHADER
	{
		Shader *shader = new Shader("VertexColor");
		shader->AddShader("Shaders/defaultVertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Assets/Shaders/VertexColor.FS.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}
}

std::vector<glm::vec3> ggColors{
	glm::vec3(0,0.2,1), glm::vec3(1,0.5,0), glm::vec3(0.5,0.5,0), glm::vec3(0,1,0), glm::vec3(0,1, 0.5), glm::vec3(0,0.5,1), glm::vec3(0,0,1), glm::vec3(0,1,1),
	glm::vec3(1,0,0), glm::vec3(1,0.5,0), glm::vec3(0.5,0.5,0), glm::vec3(0,1,0), glm::vec3(0,1, 0.5), glm::vec3(0,0.5,1), glm::vec3(0,0,1), glm::vec3(0,1,1),
	glm::vec3(1,0,0), glm::vec3(1,0.5,0), glm::vec3(0.5,0.5,0), glm::vec3(0,1,0), glm::vec3(0,1, 0.5), glm::vec3(0,0.5,1), glm::vec3(0,0,1), glm::vec3(0,1,1),
	glm::vec3(1,0,0), glm::vec3(1,0.5,0), glm::vec3(0.5,0.5,0), glm::vec3(0,1,0), glm::vec3(0,1, 0.5), glm::vec3(0,0.5,1), glm::vec3(0,0,1), glm::vec3(0,1,1)
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MeshVoxelizer::~MeshVoxelizer()
{
	//distruge shader
	//distruge mesh incarcat
	delete lineMesh;
	delete pointMesh;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MeshVoxelizer::LoadMaterials()
{ 

	buttonUp= new Texture2D();
	buttonUp->Load2D("Assets/buttonUp.png", GL_REPEAT);

	buttonDown = new Texture2D();
	buttonDown->Load2D("Assets/buttonDown.png", GL_REPEAT);
	
	buttonDisabled = new Texture2D();
	buttonDisabled->Load2D("Assets/buttonDisabled.png", GL_REPEAT);
	

	displayShadedPic = new Texture2D();
	displayShadedPic->Load2D("Assets/displayShaded.png", GL_REPEAT);
	displayNormalsPic = new Texture2D();
	displayNormalsPic->Load2D("Assets/displayNormals.png", GL_REPEAT);
	displayPatchesPic = new Texture2D();
	displayPatchesPic->Load2D("Assets/displayPatches.png", GL_REPEAT);
	displaySobelPic = new Texture2D();
	displaySobelPic->Load2D("Assets/displaySobel.png", GL_REPEAT);
	displayVertsPic = new Texture2D();
	displayVertsPic->Load2D("Assets/displayVerts.png", GL_REPEAT);
	displayEdgesPic = new Texture2D();
	displayEdgesPic->Load2D("Assets/displayEdges.png", GL_REPEAT);
	changeBGPic = new Texture2D();
	changeBGPic->Load2D("Assets/changeBackground.png", GL_REPEAT);
	m_sprite = new Sprite(shaders["FullScreenShader"], &m_width, &m_height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MeshVoxelizer::LoadMeshes()
{
	lineMesh = generateLineMesh();
	pointMesh = generatePointMesh();
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MeshVoxelizer::Init()
{
	LoadMaterials();
	LoadMeshes();
}


void MeshVoxelizer::DrawLine(Shader *shader, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &color, int lineWidth)
{
	glLineWidth(lineWidth);
	float dist = glm::distance(p2, p1);
	if (dist < 0.0001f)
		return;

	glm::mat4 scaleMat = glm::scale(glm::mat4(1), glm::vec3(dist));
	glm::mat4 rotationMat = glm::mat4(1);
	glm::mat4 translationMat = glm::translate(glm::mat4(1), glm::vec3(p1));
	glm::vec3 lineDir = glm::normalize(p2 - p1);

	float angleCosine = glm::dot(lineDir, glm::vec3(0, 1, 0));


	if (fabs(angleCosine + 1.f) < 0.0001f)
	{// 180 degrees
		rotationMat = glm::rotate(glm::mat4(1), mPI, glm::vec3(1, 0, 0));
	}
	else if (fabs(angleCosine - 1.f) > 0.0001f)
	{ //0 degrees doesn't require rot
		rotationMat = glm::rotate(glm::mat4(1), acos(angleCosine), glm::cross(glm::vec3(0, 1, 0), lineDir));
	}

	glUniformMatrix4fv(shader->GetUniformLocation(std::string("Model")),
		1, false, glm::value_ptr(translationMat * rotationMat * scaleMat));
	
	glUniform3f(shader->GetUniformLocation(std::string("color")), color.r, color.g, color.b);
	lineMesh->draw(GL_LINES);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MeshVoxelizer::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, Texture2D* texture1, Texture2D* texture2, glm::vec3 color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(view_matrix));

	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection_matrix));

	int loc_eye_pos = glGetUniformLocation(shader->program, "eye_pos");
	if(loc_eye_pos >= 0)
		glUniform3f(loc_eye_pos, camera.m_pos.x, camera.m_pos.y, camera.m_pos.z);

	if (texture1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->GetProgramID(), "texture1"), 0);
	}

	int colLoc = glGetUniformLocation(shader->GetProgramID(), "color");
	if (colLoc >= 0)
	{
		glUniform3f(colLoc, color.x, color.y, color.z);
	}
	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MeshVoxelizer::RenderBody()
{
	glEnable(GL_DEPTH_TEST);

	glLineWidth(2);
	glm::mat4 modelMatrix = glm::mat4(1);// glm::scale(glm::mat4(1), glm::vec3(0.5f));

	Shader *shader = shaders["default"];
	shader->Use();
	

	glUniform1i(glGetUniformLocation(shaders["default"]->GetProgramID(), "mode"), 0);
	glUniform1i(glGetUniformLocation(shaders["default"]->GetProgramID(), "invertColor"), 0);

	glm::mat4 mdl = glm::mat4(20);
	std::vector<glm::vec3> &pc_pts = crtChr->pc_points;
	std::vector<glm::vec3> &pc_colors = crtChr->pc_colors;
	std::vector<std::vector<glm::vec3> > &Yvoxels = crtChr->Yvoxels;
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
		

	// Bind view matrix

	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(view_matrix));

	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		
	int loc_eye_pos = glGetUniformLocation(shader->program, "eye_pos");
	glUniform3f(loc_eye_pos, camera.m_pos.x, camera.m_pos.y, camera.m_pos.z);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	int colLoc = glGetUniformLocation(shader->GetProgramID(), "color");
		
	glUniform3f(colLoc, .5f, .5f, .5f);
	
	glBindVertexArray(cubeMesh->GetBuffers()->VAO);

	float *matptr = &mdl[0][0];

	glBindVertexArray(crtChr->mVoxelMesh->GetBuffers()->VAO);
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
	glDrawElements(GL_TRIANGLES, crtChr->indexLayerStops[crtChr->currentLayer], GL_UNSIGNED_INT, 0);
}

void MeshVoxelizer::FrameStart()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MeshVoxelizer::Update(float deltaTimeSeconds)
{
	view_matrix = camera.GetViewMatrix();
	m_deltaTime = deltaTimeSeconds;

	colorPickingFB.unbind();
	glEnable(GL_DEPTH_TEST);
	
	glLineWidth(3);
	glViewport(m_width / GUI_FRACTION , 0, m_width - m_width / GUI_FRACTION, m_height);
	glClearColor(backgroundColors[backgroundID].r, backgroundColors[backgroundID].g, backgroundColors[backgroundID].b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	RenderBody();

	RenderImGUI();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////// COLOR PICKING FB ///////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	colorPickingFB.bind();
	glClearColor(0.0f, 0.0f, 0.0f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(m_width / GUI_FRACTION, 0, m_width - m_width / GUI_FRACTION , m_height);
	glEnable(GL_DEPTH_TEST);
	

	colorPickingFB.unbind();
	
	RenderButtons();
	
	colorPickingFB.bind();
	
	glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, readPixels);
	
	glDeleteTextures(1, &quadTexture);
	
	quadTexture = loadTexture(readPixels, m_width, m_height);
	
	colorPickingFB.unbind();
	
}

void MeshVoxelizer::FrameEnd() {}