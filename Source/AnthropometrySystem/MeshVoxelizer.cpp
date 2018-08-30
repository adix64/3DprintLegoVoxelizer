#include "MeshVoxelizer.h"
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include "DisjointSets.hpp"
#include <Core/Engine.h>
#include <algorithm>
#include <chrono>
#include "../include/dirent.h"
#define VOXELIZER_IMPLEMENTATION
#include <Component/voxelizer.h>



#include <stb/stb_image.h>
#include <stb/stb_image_write.h>



bool runAnthropometryAlgorithm = true;

MeshVoxelizer::MeshVoxelizer()
{
	m_width = 800; m_height = 450;
	glClearDepth(1);
	glEnable(GL_DEPTH_TEST);
	LoadShaders();
	cubeMesh = new Mesh("cube");
	cubeMesh->LoadMesh("Assets/Models/Primitives", "cube.obj");
	skyboxMesh = new Mesh("skybox");
	skyboxMesh->LoadMesh("Assets/Models/Primitives", "skybox.obj", 1000);

	camPivot = glm::vec3(0);
	model_matrix = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	view_matrix = glm::lookAt(glm::vec3(-5, 10, 75), glm::vec3(5, 10, 0), glm::vec3(0, 1, 0));

	//wireframe draw mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	camera = Camera(glm::vec3(0, 60, 80), glm::vec3(0, 30, 0), glm::vec3(0, 1, 0));
	memset(keyStates, 0, 256);

	colorPickingFB.generate(m_width, m_height);

	readPixels = (GLubyte*)malloc(3 * m_width * m_height);

	fsQuad = new Sprite(shaders["FullScreenShader"], &m_width, &m_height, glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0));
	textSprite = new Sprite(shaders["FullScreenShader"], &m_width, &m_height, glm::vec3(-1, -1, 0), glm::vec3(1, 1, 0));


	std::vector<glm::uvec3> reservedColors = {
		glm::uvec3(255, 0, 0), glm::uvec3(0, 255, 0), glm::uvec3(0, 0, 255),
		glm::uvec3(255, 255, 0), glm::uvec3(255, 0, 255), glm::uvec3(0, 127, 127),
		glm::uvec3(127, 0, 0), glm::uvec3(0, 127, 0), glm::uvec3(0, 0, 127),
		glm::uvec3(127, 127, 0), glm::uvec3(127, 0, 127), glm::uvec3(0, 127, 127),
		glm::uvec3(0, 128, 128), glm::uvec3(128, 0, 0), glm::uvec3(0, 128, 0),
		glm::uvec3(0, 0, 128), glm::uvec3(128, 128, 0), glm::uvec3(128, 0, 128),
		glm::uvec3(0, 128, 128), glm::uvec3(64, 64, 64), glm::uvec3(64, 64, 127),
		glm::uvec3(64, 127, 127), glm::uvec3(64, 127, 191),	glm::uvec3(191, 127, 191),
		glm::uvec3(191, 0, 191), glm::uvec3(191, 64, 191)
	};
	colorGen.SetReservedColors(reservedColors);
	mTextRenderer.Init(shaders["Text"], "Assets/Fonts/crkdownr.ttf");
	mTextOutliner.Init(shaders["Text"], "Assets/Fonts/crkdwno2.ttf");

	/*char num[8];
	DIR *dir;
	struct dirent *ent;
	
	if ((dir = opendir((RESOURCE_PATH::MODELS + "Characters").c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL) {
			if (!strcmp(ent->d_name, "."))
				continue;
			if (!strcmp(ent->d_name, ".."))
				continue;
			
			InitBodyModel((RESOURCE_PATH::MODELS + "Characters").c_str(), ent->d_name);
			break;
		}
		closedir(dir);
	}*/
	InitBodyModel("Assets/Models/Jerry", "jerry.obj", "jerry.png", 6);
	//InitBodyModel("Assets/Models", "batman.obj", "batman.png", 1);
	//InitBodyModel("Assets/Models/Tom", "tom.obj", "tom.png", 6);
	//InitBodyModel("Assets/Models/Spike", "spike.obj", "spike.png", 4);
	
	OnWindowResize(800, 450);
	

	InitTheGUI();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MeshVoxelizer::InitBodyModel(const char *fileLocation, const char* fileName, const char *texture, float scale)
{
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	Mesh* mesh = new Mesh(fileName);
//load the character mesh...
	mesh->LoadMesh(fileLocation, fileName, scale);
	meshes[mesh->GetMeshID()] = mesh;
	mCharacters.push_back(new MeasureableCharacter());
	crtChr = mCharacters[mCharacters.size() - 1];
	crtChr->mName = fileName;
	crtChrIDX = mCharacters.size() - 1;
	crtChr->mMesh = mesh;
	crtChr->mNumVerts = mesh->positions.size();

	crtChr->textureData = stbi_load((std::string(fileLocation) + "/" + texture).c_str(), &(crtChr->texW), &(crtChr->texH), &(crtChr->texCh), 0);

	cout << endl << "______________________________________________________"
		<< endl << "Measuring " << fileName << "  ...  Vertex count: " << crtChr->mNumVerts << endl;
//
//find extremities - AABB
	crtChr->aabbMin = glm::vec3(999999);
	crtChr->aabbMax = glm::vec3(-999999);

	for (int i = 0; i < mesh->positions.size(); i++)
	{
		if (mesh->positions[i].x < crtChr->aabbMin.x)
			crtChr->aabbMin.x = mesh->positions[i].x;
		else if (mesh->positions[i].x > crtChr->aabbMax.x)
			crtChr->aabbMax.x = mesh->positions[i].x;

		if (mesh->positions[i].y < crtChr->aabbMin.y)
			crtChr->aabbMin.y = mesh->positions[i].y;
		else if (mesh->positions[i].y > crtChr->aabbMax.y)
			crtChr->aabbMax.y = mesh->positions[i].y;

		if (mesh->positions[i].z < crtChr->aabbMin.z)
			crtChr->aabbMin.z = mesh->positions[i].z;
		else if (mesh->positions[i].z > crtChr->aabbMax.z)
			crtChr->aabbMax.z = mesh->positions[i].z;
		//construct vertex graph
		crtChr->vertexGraph[i] = std::set<GLuint>();
	}
//
	for (int i = 0; i < mesh->indices.size(); i += 3)
	{//construct vertex graph with neighbors
		crtChr->vertexGraph[mesh->indices[i]].emplace(mesh->indices[i + 1]);
		crtChr->vertexGraph[mesh->indices[i]].emplace(mesh->indices[i + 2]);

		crtChr->vertexGraph[mesh->indices[i + 1]].emplace(mesh->indices[i]);
		crtChr->vertexGraph[mesh->indices[i + 1]].emplace(mesh->indices[i + 2]);

		crtChr->vertexGraph[mesh->indices[i + 2]].emplace(mesh->indices[i]);
		crtChr->vertexGraph[mesh->indices[i + 2]].emplace(mesh->indices[i + 1]);
	}

	VoxelizeBodyModel(0.5);

	camPivot = glm::vec3(0, crtChr->aabbMax.y - (crtChr->aabbMin.y + crtChr->aabbMax.y) * .333f,0);

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds > (t2 - t1).count();
	cout << "Processing time : " << duration << "ms" << endl 
		<< "______________________________________________________" << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MeshVoxelizer::VoxelizeBodyModel(float res)
{
	
	//std::ofstream file((RESOURCE_PATH::MODELS + "mesh_voxelized_res.obj").c_str());


	//size_t voffset = 0;
	//size_t noffset = 0;


	
	vx_mesh_t *vxmesh = vx_mesh_alloc(crtChr->mMesh->positions.size(), crtChr->mMesh->indices.size());

	for (size_t f = 0; f < crtChr->mMesh->indices.size(); f++) {
		vxmesh->indices[f] = crtChr->mMesh->indices[f];
	}
	for (size_t v = 0; v < crtChr->mMesh->positions.size(); v++) {
		vxmesh->vertices[v].x = crtChr->mMesh->positions[v].x;
		vxmesh->vertices[v].y = crtChr->mMesh->positions[v].y;
		vxmesh->vertices[v].z = crtChr->mMesh->positions[v].z;
		vxmesh->colors[v].x = crtChr->mMesh->texCoords[v].x;
		vxmesh->colors[v].y = crtChr->mMesh->texCoords[v].y;
	}
	
	float precision = .01;

	/*vx_mesh_t* result;
	result = vx_voxelize(mesh, res, res, res, precision);
*/
	vx_point_cloud_t *pc_result;
	pc_result = vx_voxelize_pc(vxmesh, res, res, res, precision);
	
	int texW = crtChr->texW, texH = crtChr->texH, numCh = crtChr->texCh;
	glm::uvec2 coords;
	unsigned char *texData = crtChr->textureData;
	size_t base_idx;
	glm::vec3 aabbMax(-999999);
	glm::vec3 aabbMin( 999999);
	crtChr->pc_points.clear();
	crtChr->pc_colors.clear();
	for (int i = 0; i < pc_result->nvertices; i++)
	{
		crtChr->pc_points.push_back(glm::vec3(pc_result->vertices[i].x,
											  pc_result->vertices[i].y,
											  pc_result->vertices[i].z));
		
		coords = { glm::clamp(pc_result->colors[i].x,0.f,1.f) * texW, 
			(1.f - glm::clamp(pc_result->colors[i].y,0.f,1.f)) * texH };

		base_idx = numCh * (coords.y *texW + coords.x);
		float pxr = texData[base_idx + 0];
		float pxg = texData[base_idx + 1];
		float pxb = texData[base_idx + 2];

		crtChr->pc_colors.push_back(glm::vec3(pxr / 255.f, pxg / 255.f, pxb / 255.f));
		if (pc_result->vertices[i].x > aabbMax.x)
			aabbMax.x = pc_result->vertices[i].x;
		if (pc_result->vertices[i].y > aabbMax.y)
			aabbMax.y = pc_result->vertices[i].y;
		if (pc_result->vertices[i].z > aabbMax.z)
			aabbMax.z = pc_result->vertices[i].z;
		if (pc_result->vertices[i].x < aabbMin.x)
			aabbMin.x = pc_result->vertices[i].x;
		if (pc_result->vertices[i].y < aabbMin.y)
			aabbMin.y = pc_result->vertices[i].y;
		if (pc_result->vertices[i].z < aabbMin.z)
			aabbMin.z = pc_result->vertices[i].z;
	}

	int dimX = (aabbMax.x - aabbMin.x) / res + 1;
	int dimY = (aabbMax.y - aabbMin.y) / res + 1;
	int dimZ = (aabbMax.z - aabbMin.z) / res + 1;

	std::vector<std::vector<glm::vec3>> &Yvoxels = crtChr->Yvoxels;
	std::vector<std::vector<glm::vec3>> &YvoxCols = crtChr->YvoxCols;
	Yvoxels.clear();
	YvoxCols.clear();
	Yvoxels.resize(dimY, std::vector<glm::vec3>());
	YvoxCols.resize(dimY, std::vector<glm::vec3>());
	int Ycoord;
	for (int i = 0; i < pc_result->nvertices; i++)
	{
		Ycoord = (pc_result->vertices[i].y - aabbMin.y) / res;
		Yvoxels[Ycoord].push_back(glm::vec3(pc_result->vertices[i].x,
										   pc_result->vertices[i].y,
										   pc_result->vertices[i].z));
		YvoxCols[Ycoord].push_back(glm::vec3(crtChr->pc_colors[i].x,
										 	 crtChr->pc_colors[i].y,
										     crtChr->pc_colors[i].z));
	}

	Mesh *layeredMesh = new Mesh("layeredMesh");
	int vindex = 0;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> vcolors;
	std::vector<glm::vec3> normals;
	std::vector<unsigned> indices;
	
	std::vector<unsigned> indexLayerStops;
	for (int i = 0; i < Yvoxels.size(); i++)
	{
		for (int j = 0; j < Yvoxels[i].size(); j++)
		{
			for (int k = 0; k < cubeMesh->positions.size(); k++)
			{
				glm::vec3 &v = Yvoxels[i][j];
				glm::vec3 transformedPos = glm::vec3(glm::mat4(res, 0, 0, 0,
																0, res, 0, 0,
																0, 0, res, 0,
																v.x, v.y, v.z, 1)
											* glm::vec4(cubeMesh->positions[k], 1));
				positions.push_back(transformedPos);
				vcolors.push_back(YvoxCols[i][j]);
				normals.push_back(cubeMesh->normals[k]);
				indices.push_back(k + vindex);
			}
			vindex += cubeMesh->positions.size();
		}
		indexLayerStops.push_back(vindex);
	}
	layeredMesh->InitFromData(positions, normals, vcolors, indices);
	if (crtChr->mVoxelMesh)
	{
		crtChr->mVoxelMesh->ClearData();
		delete crtChr->mVoxelMesh;
	}
	crtChr->mVoxelMesh = layeredMesh;
	crtChr->indexLayerStops = indexLayerStops;

	//printf("Number of vertices: %ld\n", result->nvertices);
	//printf("Number of indices: %ld\n", result->nindices);

	//if (file.is_open()) {
	//	file << "\n";
	//	//file << "o " << i << "\n";

	//	for (int j = 0; j < result->nvertices; ++j) {
	//		file << "v " << result->vertices[j].x << " "
	//			<< result->vertices[j].y << " "
	//			<< result->vertices[j].z << "\n";
	//	}

	//	for (int j = 0; j < result->nnormals; ++j) {
	//		file << "vn " << result->normals[j].x << " "
	//			<< result->normals[j].y << " "
	//			<< result->normals[j].z << "\n";
	//	}

	//	size_t max = 0;

	//	for (int j = 0; j < result->nindices; j += 3) {
	//		size_t i0 = voffset + result->indices[j + 0] + 1;
	//		size_t i1 = voffset + result->indices[j + 1] + 1;
	//		size_t i2 = voffset + result->indices[j + 2] + 1;

	//		max = i0 > max ? i0 : max;
	//		max = i1 > max ? i1 : max;
	//		max = i2 > max ? i2 : max;

	//		file << "f ";

	//		file << i0 << "//" << result->normalindices[j + 0] + noffset + 1 << " ";
	//		file << i1 << "//" << result->normalindices[j + 1] + noffset + 1 << " ";
	//		file << i2 << "//" << result->normalindices[j + 2] + noffset + 1 << "\n";
	//	}

	//	voffset += max;
	//	noffset += 6;
	//}

	vx_point_cloud_free(pc_result);
	vx_mesh_free(vxmesh);
	//file.close();
	//Mesh* vmesh = new Mesh("mesh_voxelized_res.obj");
	//vmesh->LoadMesh((RESOURCE_PATH::MODELS).c_str(),"mesh_voxelized_res.obj");
	//crtChr->mVoxelMesh = vmesh;


}


////////////////////////////////////////////////////////////////////////////////////////////

//void MeshVoxelizer::AddPlaneAtScreenPoint(glm::vec2 screenSpacePos)
//{
//	float leftLimit = (float)m_width / (float)GUI_FRACTION;
//	if (screenSpacePos.x < leftLimit)
//		return;
//	screenSpacePos.x -= leftLimit;
//	screenSpacePos.x *= 1 + leftLimit / (float)m_width;
//
//	screenSpacePos.x = screenSpacePos.x / (float)m_width * 2.f - 1.f;
//	screenSpacePos.y = (m_height - screenSpacePos.y) / (float)m_height * 2.f - 1.f;
//	
//	glm::vec4 worldSpacePos = glm::inverse(projection_matrix * view_matrix) * glm::vec4(screenSpacePos.x, screenSpacePos.y, 0.95, 1);
//
//	worldSpacePos.w = 1.0 / worldSpacePos.w;
//	worldSpacePos.x *= worldSpacePos.w;
//	worldSpacePos.y *= worldSpacePos.w;
//	worldSpacePos.z *= worldSpacePos.w;
//#pragma message "HANDLE DIVISION BY ZER(0add)!!!"
//	const glm::vec3 camPos = camera.GetPosition();
//	glm::vec3 dirVec = glm::normalize(glm::vec3(worldSpacePos) - camPos);
//	worldSpacePos = glm::vec4(camPos - dirVec * camPos.z / dirVec.z, 1); //TODO
//	crtChr->slicePlanes.push_back(new Plane(glm::vec3(worldSpacePos), glm::vec3(0,1,0)));
//	activePlane = crtChr->slicePlanes[crtChr->slicePlanes.size() - 1];
//	activePlane->colorFBOid = colorGen.getNextColor();
//	printf("[COLOR GENERATOR]: Next Color: %d %d %d \n", activePlane->colorFBOid.x, activePlane->colorFBOid.y, activePlane->colorFBOid.z);
//	ComputePlaneMeshIntersection(crtChr->mMesh, activePlane, &(crtChr->crtPerimeter), &(crtChr->crtSurfaceArea));
//	planesHashes[calculateColorHash(activePlane->colorFBOid)] = activePlane;
//}
//
