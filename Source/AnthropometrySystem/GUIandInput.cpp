#include "MeshVoxelizer.h"
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include "DisjointSets.hpp"
#include <Core/Engine.h>
#include <algorithm>

#include <chrono>

#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <imgui\imgui_setup.h>



void MeshVoxelizer::InitTheGUI()
{
	ImGui_ImplGlfwGL3_Init(window->window, false);
}

void MeshVoxelizer::RenderImGUI()
{
	ImGui_ImplGlfwGL3_NewFrame();
	// 1. Show a simple window
	bool xt = true;
	ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiSetCond_FirstUseEver);
	//ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 100, 100));
	ImGui::Begin("", &xt,ImVec2(600,600));

	ImGui::Text(crtChr->mName.c_str());
	
	if (ImGui::Button("Prev"))
	{
		crtChrIDX = (crtChrIDX - 1) % mCharacters.size();
		crtChr = mCharacters[crtChrIDX];
	}
	ImGui::SameLine();
	if (ImGui::Button("Next"))
	{
		crtChrIDX = (crtChrIDX + 1) % mCharacters.size();
		crtChr = mCharacters[crtChrIDX];
	}
	ImGui::Text("_____________________");


	ImGui::Text("Options:");
	ImGui::Checkbox("Display Voxelized", &showVoxels);
	if (ImGui::Button("Level++"))
	{
		crtChr->currentLayer = min(crtChr->Yvoxels.size() - 1, (size_t)crtChr->currentLayer + 1);
	}else
	{
		if (ImGui::IsItemActive())
		{
			if(lastLayerIncrementTime >= .5)
				crtChr->currentLayer = min(crtChr->Yvoxels.size() - 1, (size_t)crtChr->currentLayer + 1);
			lastLayerIncrementTime = min(lastLayerIncrementTime + m_deltaTime, 1.f);
		}
		else 
		{
			lastLayerIncrementTime = 0;
		}
	}
	if (ImGui::Button("Level--"))
	{
		crtChr->currentLayer = max((size_t)1, (size_t)crtChr->currentLayer - 1);
	}
	else
	{
		if (ImGui::IsItemActive())
		{
			if (lastLayerDecrementTime >= .5)
				crtChr->currentLayer = max((size_t)1, (size_t)crtChr->currentLayer - 1);
			lastLayerDecrementTime = min(lastLayerDecrementTime + m_deltaTime, 1.f);
		}
		else
		{
			lastLayerDecrementTime = 0;
		}
	}
	ImGui::Text("_____________________");

	if (ImGui::Button("Resolution++"))
	{
		crtChr->voxResolution += 0.25f;
		VoxelizeBodyModel(crtChr->voxResolution);
	}

	if (ImGui::Button("Resolution--"))
	{
		crtChr->voxResolution -= 0.25f;
		VoxelizeBodyModel(crtChr->voxResolution);
	}

	ImGui::End();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	ImGui::Render();
}

////////////////////////////////////////////////////////////////////////////////
void MeshVoxelizer::RenderButtons()
{
	glViewport(0, 0, m_width, m_height);
	
	Texture2D *shadedMeshBG = buttonUp, *normalsMeshBG = buttonUp, *patchesMeshBG = buttonUp, *sobelMeshBG = buttonUp;

			
	m_sprite->SetCorners(glm::vec3(-1, 0.2-0.05, 0), glm::vec3(-0.9, 0.1 - 0.05, 0));
	m_sprite->Render(shadedMeshBG);
	m_sprite->Render(displayShadedPic);

	m_sprite->SetCorners(glm::vec3(-1, 0.1 - 0.05, 0), glm::vec3(-0.9, 0 - 0.05, 0));
	m_sprite->Render(normalsMeshBG);
	m_sprite->Render(displayNormalsPic);

	m_sprite->SetCorners(glm::vec3(-1, 0 - 0.05, 0), glm::vec3(-0.9, -0.1 - 0.05, 0));
	m_sprite->Render(patchesMeshBG);
	m_sprite->Render(displayPatchesPic);

	m_sprite->SetCorners(glm::vec3(-1, -0.1 - 0.05, 0), glm::vec3(-0.9, -0.2 - 0.05, 0));
	m_sprite->Render(sobelMeshBG);
	m_sprite->Render(displaySobelPic);

	m_sprite->SetCorners(glm::vec3(-0.95, -0.2 - 0.1, 0), glm::vec3(-0.9, -0.3 - 0.1, 0));
	m_sprite->Render(drawBodyWireframe ? buttonDown : buttonUp);
	m_sprite->Render(displayEdgesPic);

	m_sprite->SetCorners(glm::vec3(-1, -0.3 - 0.1, 0), glm::vec3(-0.9, -0.5 - 0.1, 0));
	m_sprite->Render(buttonUp);
	m_sprite->Render(changeBGPic);

	//COLOR PICKING FB
	colorPickingFB.bind();
	m_sprite->SetCorners(glm::vec3(-1, 0.8, 0), glm::vec3(-0.9, 1, 0));
	m_sprite->Render(glm::vec3(1, 1, 0));
	
	m_sprite->SetCorners(glm::vec3(-1, 0.6, 0), glm::vec3(-0.9, 0.8, 0));
	m_sprite->Render(glm::vec3(1, 0, 1));

	m_sprite->SetCorners(glm::vec3(-1, 0.4, 0), glm::vec3(-0.9, 0.6, 0));
	m_sprite->Render(glm::vec3(1, 0, 0.5));
	
	m_sprite->SetCorners(glm::vec3(-1, 0.2, 0), glm::vec3(-0.9, 0.4, 0));
	m_sprite->Render(glm::vec3(0, 1, 1));
	//
	m_sprite->SetCorners(glm::vec3(-1, 0.2 - 0.05, 0), glm::vec3(-0.9, 0.1 - 0.05, 0));
	m_sprite->Render(glm::vec3(0.25, 0.25, 0.25));

	m_sprite->SetCorners(glm::vec3(-1, 0.1 - 0.05, 0), glm::vec3(-0.9, 0 - 0.05, 0));
	m_sprite->Render(glm::vec3(0.25, 0.25, 0.5));

	m_sprite->SetCorners(glm::vec3(-1, 0 - 0.05, 0), glm::vec3(-0.9, -0.1 - 0.05, 0));
	m_sprite->Render(glm::vec3(0.25, 0.5, 0.5));

	m_sprite->SetCorners(glm::vec3(-1, -0.1 - 0.05, 0), glm::vec3(-0.9, -0.2 - 0.05, 0));
	m_sprite->Render(glm::vec3(0.25, 0.5, 0.75));

	m_sprite->SetCorners(glm::vec3(-1, -0.2 - 0.1, 0), glm::vec3(-0.95, -0.3 - 0.1, 0));
	m_sprite->Render(glm::vec3(0.75, 0.5, 0.75));

	m_sprite->SetCorners(glm::vec3(-0.95, -0.2 - 0.1, 0), glm::vec3(-0.9, -0.3 - 0.1, 0));
	m_sprite->Render(glm::vec3(0.75, 0, 0.75));

	m_sprite->SetCorners(glm::vec3(-1, -0.3 - 0.1, 0), glm::vec3(-0.9, -0.5 - 0.1, 0));
	m_sprite->Render(glm::vec3(0.75, 0.25, 0.75));

	colorPickingFB.unbind();
#define	DEBUG_MODE_READPIXELS
#ifdef DEBUG_MODE_READPIXELS
	if (showColorPickingFB)
		fsQuad->Render(quadTexture);
#endif

}

void MeshVoxelizer::OnInputUpdate(float deltaTime, int mods)
{
	float speed = 2;

	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right = GetSceneCamera()->transform->GetLocalOXVector();
		glm::vec3 forward = GetSceneCamera()->transform->GetLocalOZVector();
		forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));
	}
}

void MeshVoxelizer::OnKeyPress(int key, int mods)
{
	if (key < 256)
		keyStates[key] = true;

	if (key == GLFW_KEY_4)
		drawBodyWireframe = !drawBodyWireframe;

	else if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT)
	{
		glfwSetInputMode(window->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_altDown = true;
	}else	
	if (key == 27)
	{
		exit(0);
	}
	else
	if (key == 32)
	{
		for(auto s : shaders)
		{
			s.second->Reload();
		}			
	}else
	if (key == GLFW_KEY_P)
	{
		showColorPickingFB = !showColorPickingFB;
	}else
	if (key == GLFW_KEY_Z)
	{
		camPivot = glm::vec3(0);
		camera = Camera(glm::vec3(0, 60, 80), glm::vec3(0, 30, 0), glm::vec3(0, 1, 0));
	}
	else if (key == GLFW_KEY_N)
	{
		crtChrIDX = (crtChrIDX + 1) % mCharacters.size();
		crtChr = mCharacters[crtChrIDX];
	}

}

void MeshVoxelizer::OnKeyRelease(int key, int mods)
{
	if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT)
	{
		glfwSetInputMode(window->GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_altDown = false;
		return;
	}
	if (key < 256)
		keyStates[key] = false;
	
}

//void MeshVoxelizer::ForceRedraw()
//{
//	FrameStart();
//	Update(0.02);
//	FrameEnd();
//	window->SwapBuffers();
//}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void MeshVoxelizer::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (m_altDown)
	{
		float dx = (float)deltaX;
		float dy = (float)deltaY;

		if (m_LMB)
		{
			camera.RotateAroundPointX(-dy * 0.00499f, camPivot);
			camera.RotateAroundPointY(-dx * 0.00499f, camPivot);

			camera.FixOZRotationYup();
		}
		else if (m_RMB)
		{
			camera.TranslateAlongZ((-dy + dx) * 0.1f);
		}
		else if (m_MMB)
		{
#define mmbspeed 0.1f
			camera.TranslateAlongX(-dx * mmbspeed);
			camera.TranslateAlongY(dy * mmbspeed);
			camPivot += glm::vec3(-dx * mmbspeed * camera.m_right + dy * mmbspeed * camera.m_up);
			camera.FixOZRotationYup();
		}
	}
	prev_mousePos = glm::ivec2(mouseX, mouseY);
}

void MeshVoxelizer::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{	
	if (button == 1)
	{
		m_LMB = true;
		if (!m_altDown)
		{
			glm::uvec3 readPx = glm::uvec3(readPixels[(m_width * (m_height - mouseY) + mouseX) * 3],
				readPixels[(m_width * (m_height - mouseY) + mouseX) * 3 + 1],
				readPixels[(m_width * (m_height - mouseY) + mouseX) * 3 + 2]);
			if (readPx.r == 191 && readPx.g == 0 && readPx.b == 191)
			{
				drawBodyWireframe = !drawBodyWireframe;
			}
			else if (readPx.r == 191 && readPx.g == 64 && readPx.b == 191)
			{
				backgroundID = (backgroundID + 1) % 6;
			}
		}
	}
	else if (button == 2)
	{
		m_RMB = true;
		if (!m_altDown) {
			//????????????????? TODO 
		}
	}
	else
		m_MMB = true;

}

void MeshVoxelizer::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	if (button == 1)
		m_LMB = false;
	else if (button == 2)
		m_RMB = false;
	else
		m_MMB = false;
}

void MeshVoxelizer::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	////glfwSetCursorPos(window->GetGLFWWindow(), m_width / 2, m_height / 2);
	//float dx = m_width / 2 - mouseX;
	//float dy = m_height / 2 - mouseY;
	camera.TranslateAlongZ(offsetY);
	//ForceRedraw();
}

//functei care e chemata cand se schimba dimensiunea ferestrei initiale
void MeshVoxelizer::OnWindowResize(int width, int height)
{
	mTextRenderer.Resize(width, height);
	mTextOutliner.Resize(width, height);
	//reshape
	if (height == 0) height = 1;
	m_width = width; m_height = height;
	float aspect = (float)width / (float)height;
	colorPickingFB.destroy();
	colorPickingFB.generate(width, height);
	free(readPixels);
	readPixels = (GLubyte*)malloc(3 * width * height);
	aspect = (float)(m_width - m_width / GUI_FRACTION) / (float)height;
	projection_matrix = glm::perspective(45.0f, aspect, 1.f, 2000.f);
}
