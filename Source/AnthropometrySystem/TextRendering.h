#pragma once
//#include <Component/SimpleScene.h>
//#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>
//#include <Core/GPU/CubeMapFBO.h>
#include <Core/GPU/Texture2D.h>
//#include <Core/GPU/ShadowCubeMapFBO.h>
//#include "Gizmo.hpp"
//#include "Camera.hpp"
//#include "Grid.hpp"
#include <Core/GPU/Framebuffer.hpp>
//#include "ColorGenerator.hpp"
#include <Core\GPU\Sprite.hpp>
//#include "DisjointSets.hpp"
#include "../../libs/freetype29/ft2build.h"
#include FT_FREETYPE_H
// GL includes
#include <Core\GPU\Shader.h>
// Properties
// const GLuint WIDTH = 800, HEIGHT = 600;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

class TextRenderer
{
public:
	std::map<GLchar, Character> Characters;
	GLuint VAO, VBO;
	Shader *mShader;
	int mWidth, mHeight;
	void Init(Shader *pShader, char *fontfile);
	void Resize(int, int);
	void _RenderText(std::string text, GLfloat x, GLfloat y, GLfloat &sizeScale, GLfloat &bearingScale, glm::vec3 &color);
	void RenderText(std::string &text, GLfloat x, GLfloat y, GLfloat sizeScale, GLfloat bearingScale, glm::vec3 &color);
};
