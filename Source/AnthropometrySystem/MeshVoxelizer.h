#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>
#include <Core/GPU/CubeMapFBO.h>
#include <Core/GPU/Texture2D.h>
#include <Core/GPU/ShadowCubeMapFBO.h>
#include "Gizmo.hpp"
#include "Camera.hpp"
#include "Grid.hpp"
#include "FullscreenQuad.hpp"
#include <Core/GPU/Framebuffer.hpp>
#include "ColorGenerator.hpp"
#include <Core\GPU\Sprite.hpp>
#include "DisjointSets.hpp"
#include "TextRendering.h"
#include <unordered_map>
#include <set>



typedef std::vector<VertexFormat> TVertexList;
typedef std::vector<uint32_t> TIndexList;
typedef std::vector<glm::vec3> CurvePointList;

inline uint64_t calculateColorHash(glm::uvec3 v)
{
	uint64_t out = 0x0000000000000000;
	out = out | v.x;
	out = out | v.y << 8;
	out = out | v.z << 16;
	return out;
}


using namespace std;
#define CENTER_SIDE 0
#define LEFT_SIDE 1
#define RIGHT_SIDE 2

struct DebugPoint { glm::vec3 pos, color; };
struct DebugLine { glm::vec3 p1, p2, color; };

struct MeasureableCharacter
{
	std::string mName;
	SimpleScene *anthroSys;
	Mesh *mMesh, *mVoxelMesh;
	std::vector<glm::vec3> pc_points, pc_colors;
	std::vector<std::vector<glm::vec3>> Yvoxels, YvoxCols;
	std::vector<unsigned> indexLayerStops;
	unsigned char *textureData;
	int texW, texH, texCh;
	glm::vec3 aabbMin, aabbMax;
	std::unordered_map<GLuint, std::set<GLuint>> vertexGraph;
	int mNumVerts;
	std::vector<DebugPoint> debugPoints;
	std::vector<DebugLine> debugLines;
	int currentLayer = 1;
};

class MeshVoxelizer : public SimpleScene
{
	public:
		MeshVoxelizer();
		~MeshVoxelizer();

		void Init() override;
		void InitTheGUI();
	private:
		void LoadMaterials();
		void LoadMeshes();
		void LoadShaders();

		void MeasureBodyModel();
		void InitBodyModel(const char* fileLocation, const char* fileName, const char *texture = NULL, float scale = 1.f);

		void FrameStart() override;
		void RenderBody();
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;
		void RenderImGUI();

		void DrawLine(Shader *shader, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &color, int lineWidth);
		void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, Texture2D* texture1 = NULL, Texture2D* texture2 = NULL, glm::vec3 color = glm::vec3(0, 0, 0));
		
		//void ForceRedraw();

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;
		void RenderButtons();
private:
	
	glm::vec3 camPivot;
	Camera camera;

	glm::mat4 model_matrix, view_matrix, projection_matrix;
	BaseMesh *lineMesh;
	BaseMesh *gizmoLine, *gizmoCone;
	
	BaseMesh *pointMesh;
	Mesh *cubeMesh;

	Sprite *fsQuad, *textSprite;
	TextRenderer mTextRenderer, mTextOutliner;
	int selectedIndex = -1;

	bool keyStates[256];
	bool m_LMB = false, m_RMB = false, m_MMB = false;
	bool m_altDown = false;

	float m_deltaTime;
	int m_width = 1280, m_height = 720;

	
	
	lab::Framebuffer colorPickingFB;
	GLubyte *readPixels;
	unsigned int quadTexture;
	glm::vec3 gizmoPos;
	glm::ivec2 prev_mousePos;
	glm::vec2 prev_ssdir = glm::vec2(0, 0);
	ColorGenerator colorGen;
	Texture2D *buttonUp, *buttonDown, *buttonDisabled;
	Texture2D *displayShadedPic, *displayNormalsPic, *displayPatchesPic, *displaySobelPic, *displayVertsPic, *displayEdgesPic, *changeBGPic;
	Sprite *m_sprite;
	bool showColorPickingFB = false;

	ColorGenerator colorGenerator;
	
	
	std::unordered_map<uint64_t, glm::vec3*> planesHashes;

	
	//int mNumIters = 3;
	float mIterStep=0.005f, mThreshold = 0.99f;


	float outlinerSizeScale = .61f, outlinerBearingScale = .52f,
		textSizeScale = .5f, textBearingScale = .5f,
		xoffsettextoutline = -.006f, yoffsettextoutline = -0.01f;
	
	std::vector<MeasureableCharacter*> mCharacters;
	MeasureableCharacter* crtChr;
	int crtChrIDX;
	/////////////////////////these were globals
	bool showVoxels = false;
	bool drawBodyWireframe = true;
	int backgroundID = 0;
	float lastLayerIncrementTime = 0;
	float lastLayerDecrementTime = 0;
};


int FurthestPointAlongDir(std::vector<glm::vec3> &positions, std::vector<unsigned int> &indices, glm::vec3 &direction);

int FurthestPointAlongDir(std::unordered_map<int, glm::vec3> &verts, glm::vec3 &direction);

int IDofClosestCentroidToPoint(std::unordered_map<int, glm::vec3> &centroids, glm::vec3 pPoint, float thresh = 99999.f);


#define GUI_FRACTION 16
#define mPI 3.1415926f