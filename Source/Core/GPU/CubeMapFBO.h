#pragma once


#include <include/gl.h>
// GLM


#ifndef CUBE_MAP_FBO_H
#define	CUBE_MAP_FBO_H


class CubeMapFBO
{
public:
	CubeMapFBO();

	~CubeMapFBO();

	bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

	void BindForWriting(GLenum face,  int i);

	void BindForReading(GLenum TextureUnit, int uniform_location);

private:
	GLuint m_fbo;
	GLuint m_shadowMap;
	GLuint m_depth;
};

#endif	/* CUBE_MAP_FBO_H */

