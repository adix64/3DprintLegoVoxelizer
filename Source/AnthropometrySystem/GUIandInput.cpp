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
	ImGui::Begin("", &xt);

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

	ImGui::Text("_____________________");
	ImGui::Text("Options:");
	ImGui::Checkbox("Display Voxelized", &showVoxels);
	if (ImGui::Button("Up"))
	{
		crtChr->currentLayer = min(crtChr->Yvoxels.size() - 1, (size_t)crtChr->currentLayer + 1);
	}
	if (ImGui::Button("Down"))
	{
		crtChr->currentLayer = max((size_t)1, (size_t)crtChr->currentLayer - 1);
	}
	//ImGui::InputFloat("outlinerSizeScale", &outlinerSizeScale, 0.01f);
	//ImGui::InputFloat("outlinerBearingScale", &outlinerBearingScale, 0.01f);
	//ImGui::InputFloat("textSizeScale", &textSizeScale, 0.01f);
	//ImGui::InputFloat("textBearingScale", &textBearingScale, 0.01f);
	//ImGui::InputFloat("Y offset", &yoffsettextoutline, 0.002f);
	//ImGui::InputFloat("X offset", &xoffsettextoutline, 0.002f);

	ImGui::End();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	ImGui::Render();
}

////////////////////////////////////////////////////////////////////////////////
void MeshVoxelizer::RenderButtons()
{
	glViewport(0, 0, m_width, m_height);
	
	Texture2D *selectBtnBG = buttonUp, *moveBtnBG = buttonUp, *planeSliceBtnBG = buttonUp, *rotateBtnBG = buttonUp;
	Texture2D *shadedMeshBG = buttonUp, *normalsMeshBG = buttonUp, *patchesMeshBG = buttonUp, *sobelMeshBG = buttonUp;
	switch(toolType)
	{
	case SELECT_TOOL:
		selectBtnBG = buttonDown;
		break;
	case MOVE_TOOL:
		moveBtnBG = buttonDown;
		break;
	case ROTATE_TOOL:
		rotateBtnBG = buttonDown;
		break;
	case PLANE_SLICE_TOOL:
		planeSliceBtnBG = buttonDown;
		break;
	default:
		break;
	}

	switch (bodyDrawMode)
	{
	case 0:
		shadedMeshBG = buttonDown;
		break;
	case 1:
		normalsMeshBG = buttonDown;
		break;
	case 2:
		patchesMeshBG = buttonDown;
		break;
	case 3:
		sobelMeshBG = buttonDown;
		break;
	default:
		shadedMeshBG = buttonDown;
		break;
	}

	


	m_sprite->SetCorners(glm::vec3(-1, 0.8, 0), glm::vec3(-0.9, 1, 0));
	m_sprite->Render(selectBtnBG);
	m_sprite->Render(selectToolPic);

	m_sprite->SetCorners(glm::vec3(-1, 0.6, 0), glm::vec3(-0.9, 0.8, 0));
	m_sprite->Render(moveBtnBG);
	m_sprite->Render(moveToolPic);

	m_sprite->SetCorners(glm::vec3(-1, 0.4, 0), glm::vec3(-0.9, 0.6, 0));
	m_sprite->Render(rotateBtnBG);
	m_sprite->Render(rotateToolPic);


	m_sprite->SetCorners(glm::vec3(-1, 0.2, 0), glm::vec3(-0.9, 0.4, 0));
	m_sprite->Render(planeSliceBtnBG);
	m_sprite->Render(planeSliceToolPic);

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

	m_sprite->SetCorners(glm::vec3(-1, -0.2 - 0.1, 0), glm::vec3(-0.95, -0.3 - 0.1, 0));
	m_sprite->Render(drawBodyPoints?buttonDown:buttonUp);
	m_sprite->Render(displayVertsPic);

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

	if (key == GLFW_KEY_1)
		bodyDrawMode = 0;
	else if (key == GLFW_KEY_2)
		bodyDrawMode = 1;
	else if (key == GLFW_KEY_3)
		bodyDrawMode = 2;
	else if (key == GLFW_KEY_4)
		drawBodyWireframe = !drawBodyWireframe;
	else if (key == GLFW_KEY_5)
		drawBodyPoints = !drawBodyPoints;
	else if (key == GLFW_KEY_6)
		drawFeatureMap = !drawFeatureMap;
	//else if (key == GLFW_KEY_M)
	//{
	//	mNumIters++;
	//	printf("NUM ITERS: %d\n", mNumIters);
	//	BuildFeatureMap(crtChr->mMesh, meshes["male1"], crtChr->vertexGraph, mNumIters, mIterStep, mThreshold);
	//}
	else
	if (key == GLFW_KEY_DELETE)
	{
/////////////////////////////
	}else

	if (key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT)
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
	else if (key == GLFW_KEY_Q)
	{
		toolType = SELECT_TOOL;
	}
	else if (key == GLFW_KEY_I)
	{
		invertColor = !invertColor;
	}
	else if (key == GLFW_KEY_W)
	{
		toolType = MOVE_TOOL;
		gizmo->crtMode = Gizmo::GizmoMode::MOVE_MODE;
	}
	else if (key == GLFW_KEY_E)
	{
		toolType = ROTATE_TOOL;
		gizmo->crtMode = Gizmo::GizmoMode::ROTATE_MODE;
	}
	else if (key == GLFW_KEY_S)
	{
		toolType = PLANE_SLICE_TOOL;
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
			//camera.RotateAroundPointX(-dy * 0.00499f, camPivot);
			//camera.RotateAroundPointY(-dx * 0.00499f , camPivot);

			if (activePlane == NULL)
			{
				camera.RotateAroundPointX(-dy * 0.00499f, camPivot);
				camera.RotateAroundPointY(-dx * 0.00499f, camPivot);
			}
			else
			{
				camera.RotateAroundPointX(-dy * 0.00499f, gizmoPos);// activePlane->position);
				camera.RotateAroundPointY(-dx * 0.00499f, gizmoPos);// activePlane->position);
			}
			camera.FixOZRotationYup();
			//ForceRedraw();
		}
		else if (m_RMB)
		{
			camera.TranslateAlongZ((-dy + dx) * 0.1f);
			//ForceRedraw();
		}
		else if (m_MMB)
		{
#define mmbspeed 0.1f
			camera.TranslateAlongX(-dx * mmbspeed);
			camera.TranslateAlongY(dy * mmbspeed);
			camPivot += glm::vec3(-dx * mmbspeed * camera.m_right + dy * mmbspeed * camera.m_up);
			camera.FixOZRotationYup();
			//ForceRedraw();
		}
		
		//glfwSetCursorPos(window->GetGLFWWindow(), m_width / 2, m_height / 2);
	}
	//else
	//{
	//	float dx = mouseX - prev_mousePos.x;
	//	float dy = prev_mousePos.y - mouseY;
		
	//	if (activePlane != NULL)
	//	{
	//		glm::vec2 dir = glm::vec2(mouseX, -mouseY) - glm::vec2(prev_mousePos.x, -prev_mousePos.y);
	//		if (gizmo->getSelectedX())
	//		{
	//			glm::vec2 ssdir = glm::vec2(projection_matrix * view_matrix * glm::translate(glm::mat4(1), gizmoPos) * glm::vec4(-1, 0, 0, 1));
	//			ssdir -= glm::vec2(projection_matrix * view_matrix * glm::translate(glm::mat4(1), gizmoPos) * glm::vec4(-0.5, 0, 0, 1));
	//	
	//			float dirlen = sqrt(glm::length(dir));
	//			float ssdirlen = glm::length(ssdir);
	//			if (dirlen > 0.001 && ssdirlen > 0.001) {
	//				float dotprod = glm::dot(glm::normalize(dir), glm::normalize(ssdir));// - prev_ssdir));
	//				
	//				if (gizmo->crtMode == Gizmo::GizmoMode::MOVE_MODE)
	//					activePlane->point = glm::vec3(glm::translate(glm::mat4(1), glm::vec3(-dirlen * dotprod, 0, 0)) *
	//						glm::vec4(activePlane->point, 1));
	//				else
	//					activePlane->normal = glm::vec3(glm::rotate(glm::mat4(1), dirlen * dotprod* 0.1f, glm::vec3(1, 0, 0)) * glm::vec4(activePlane->normal, 0));
	//				
	//				gizmoPos = activePlane->point;
	//			}
	//			prev_ssdir = ssdir;

	//		}
	//		else if (gizmo->getSelectedY())
	//		{
	//			glm::vec2 ssdir = glm::vec2(projection_matrix * view_matrix * glm::translate(glm::mat4(1), gizmoPos) * glm::vec4(0, 1, 0, 1));
	//			ssdir -= glm::vec2(projection_matrix * view_matrix * glm::translate(glm::mat4(1), gizmoPos) * glm::vec4(0, 0.5, 0, 1));
	//	
	//			float dirlen = sqrt(glm::length(dir));
	//			float ssdirlen = glm::length(ssdir);
	//			if (dirlen > 0.001 && ssdirlen > 0.001)
	//			{
	//				float dotprod = glm::dot(glm::normalize(dir), glm::normalize(ssdir));// - prev_ssdir));
	//				if (gizmo->crtMode == Gizmo::GizmoMode::MOVE_MODE)
	//					activePlane->point = glm::vec3(glm::translate(glm::mat4(1), glm::vec3(0, dirlen * dotprod, 0)) *
	//						glm::vec4(activePlane->point, 1));
	//				else
	//					activePlane->normal = glm::vec3(glm::rotate(glm::mat4(1), -dirlen * dotprod* 0.1f, glm::vec3(0, 1, 0)) * glm::vec4(activePlane->normal, 0));
	//				gizmoPos = activePlane->point;
	//			}
	//			prev_ssdir = ssdir;
	//		}
	//		else if (gizmo->getSelectedZ())
	//		{
	//			glm::vec2 ssdir = glm::vec2(projection_matrix * view_matrix * glm::translate(glm::mat4(1), gizmoPos) * glm::vec4(0, 0, 1, 1));
	//			ssdir -= glm::vec2(projection_matrix * view_matrix * glm::translate(glm::mat4(1), gizmoPos) * glm::vec4(0, 0, 0.5, 1));
	//		
	//			float dirlen = sqrt(glm::length(dir));
	//			float ssdirlen = glm::length(ssdir);
	//			if (dirlen > 0.001 && ssdirlen > 0.001) {
	//				float dotprod = glm::dot(glm::normalize(dir), glm::normalize(ssdir));// - prev_ssdir));

	//				if (gizmo->crtMode == Gizmo::GizmoMode::MOVE_MODE)
	//					activePlane->point = glm::vec3(glm::translate(glm::mat4(1), glm::vec3(0,0,dirlen * dotprod)) *
	//						glm::vec4(activePlane->point, 1));
	//				else
	//					activePlane->normal = glm::vec3(glm::rotate(glm::mat4(1), -dirlen * dotprod * 0.1f, glm::vec3(0, 0, 1)) * glm::vec4(activePlane->normal, 0));
	//				gizmoPos = activePlane->point;
	//			}
	//			prev_ssdir = ssdir;
	//		}
	//	}
	//}	
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
			if (readPx.r == 255 && readPx.g == 0 && readPx.b == 0)
			{
				gizmo->setSelectedX(true);
			}
			else if (readPx.r == 0 && readPx.g == 255 && readPx.b == 0)
			{
				gizmo->setSelectedY(true);
			}
			else if (readPx.r == 0 && readPx.g == 0 && readPx.b == 255)
			{
				gizmo->setSelectedZ(true);
			}
			else if (readPx.r == 255 && readPx.g == 255 && readPx.b == 0)
			{
				toolType = SELECT_TOOL;
				//gizmo->SetVisible(false);
			}
			else if (readPx.r == 255 && readPx.g == 0 && readPx.b == 255)
			{
				toolType = MOVE_TOOL;
				gizmo->crtMode = Gizmo::GizmoMode::MOVE_MODE;
			}
			else if (readPx.r == 255 && readPx.g == 0 && readPx.b == 127)
			{
				toolType = ROTATE_TOOL;
				gizmo->crtMode = Gizmo::GizmoMode::ROTATE_MODE;
			}
			else if (readPx.r == 0 && readPx.g == 255 && readPx.b == 255)
			{
				toolType = PLANE_SLICE_TOOL;
				gizmo->SetVisible(false);
			}
			//m_sprite->Render(glm::vec3(0.25, 0.25, 0.25));
			//m_sprite->Render(glm::vec3(0.25, 0.25, 0.5));
			//m_sprite->Render(glm::vec3(0.25, 0.5, 0.5));
			//m_sprite->Render(glm::vec3(0.25, 0.5, 0.75));

			else if (readPx.r == 64 && readPx.g == 64 && readPx.b == 64)
			{
				bodyDrawMode = 0;
			}
			else if (readPx.r == 64 && readPx.g == 64 && readPx.b == 127)
			{
				bodyDrawMode = 1;
			}
			else if (readPx.r == 64 && readPx.g == 127 && readPx.b == 127)
			{
				bodyDrawMode = 2;
				invertColor = false;
			}
			else if (readPx.r == 64 && readPx.g == 127 && readPx.b == 191)
			{
				bodyDrawMode = 3;
				invertColor = true;
			}
			else if (readPx.r == 191 && readPx.g == 127 && readPx.b == 191) 
			{
				drawBodyPoints = !drawBodyPoints;
			}
			else if (readPx.r == 191 && readPx.g == 0 && readPx.b == 191)
			{
				drawBodyWireframe = !drawBodyWireframe;
			}
			else if (readPx.r == 191 && readPx.g == 64 && readPx.b == 191)
			{
				backgroundID = (backgroundID + 1) % 6;
			}
			else
			{

				if (toolType == SELECT_TOOL)
				{
					uint64_t id = calculateColorHash(readPx);
					std::unordered_map<uint64_t, glm::vec3*>::iterator it = planesHashes.find(id);
					if (it != planesHashes.end())
					{
						activePlane = it->second;
						selectedIndex = calculateColorHash(readPx);
						gizmoPos = *activePlane;
					}
					else activePlane = NULL;
					/////////////////////////////////////////////////////////////////
				}
				else if (toolType == MOVE_TOOL || toolType == ROTATE_TOOL)
				{
					uint64_t id = calculateColorHash(readPx);
					std::unordered_map<uint64_t, glm::vec3*>::iterator it = planesHashes.find(id);
					if (it != planesHashes.end())
					{
						activePlane = it->second;
						selectedIndex = calculateColorHash(readPx);
						gizmoPos = *activePlane;
					}
					else activePlane = NULL;
				}
				else if(toolType == PLANE_SLICE_TOOL)
				{
					//AddPlaneAtScreenPoint(glm::vec2(mouseX, mouseY));
					if(activePlane)
						gizmoPos = *activePlane;
				}
			}
		}
		else
		{
			//glfwSetCursorPos(window->GetGLFWWindow(), m_width / 2, m_height / 2);
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
	gizmo->setSelectedX(false);
	gizmo->setSelectedY(false);
	gizmo->setSelectedZ(false);
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
	projection_matrix = glm::perspective(45.0f, aspect, 1.f, 1000.f);
}
