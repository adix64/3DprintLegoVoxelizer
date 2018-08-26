#pragma once
#include <include/glm.h>

class Camera
{
public:
	glm::vec3 m_pos, m_fwd, m_right, m_up;
	glm::mat4 m_view;
public:
	Camera() : m_fwd(glm::vec3(0, 0, 1)), m_up(0, 1, 0), m_right(glm::normalize(glm::cross(m_fwd, m_up))), m_view(glm::mat4(1)) {}
	Camera(glm::vec3 &pos, glm::vec3 &tgt, glm::vec3 &up) :
		m_pos(pos), m_up(glm::normalize(up)), m_fwd(glm::normalize(tgt - pos))
	{
		m_right = glm::normalize(glm::cross(m_fwd, m_up));
		m_view = glm::lookAt(pos, tgt, up);
	}

	void TranslateAlongZ(float moveSpeed)
	{
		m_pos = m_pos + moveSpeed * m_fwd;
		m_view = glm::lookAt(m_pos, m_pos + m_fwd, m_up);
	}

	void TranslateAlongX(float moveSpeed)
	{
		m_pos = m_pos + moveSpeed * m_right;
		m_view = glm::lookAt(m_pos, m_pos + m_fwd, m_up);
	}
	void TranslateAlongY(float moveSpeed)
	{
		m_pos = m_pos + moveSpeed * m_up;
		m_view = glm::lookAt(m_pos, m_pos + m_fwd, m_up);
	}

	void TranslateFWD(float moveSpeed)
	{
		m_pos = m_pos + moveSpeed * m_up;
		m_view = glm::lookAt(m_pos, m_pos + m_fwd, m_up);
	}

	void RotateAroundY1stPers(float rotAngle)
	{
		m_fwd = glm::vec3(glm::rotate(glm::mat4(1), rotAngle, glm::vec3(0, 1, 0)) * glm::vec4(m_fwd, 1));
		m_up = glm::vec3(glm::rotate(glm::mat4(1), rotAngle, glm::vec3(0, 1, 0)) * glm::vec4(m_up, 1));
		m_right = glm::normalize(glm::cross(m_fwd, m_up));
		m_view = glm::lookAt(m_pos, m_pos + m_fwd, m_up);
	}

	void RotateAroundX1stPers(float rotAngle)
	{
		m_fwd = glm::vec3(glm::rotate(glm::mat4(1), rotAngle, m_right) * glm::vec4(m_fwd, 1));
		m_up = glm::normalize(glm::cross(m_right, m_fwd));
		m_view = glm::lookAt(m_pos, m_pos + m_fwd, m_up);
	}

	void RotateAroundOriginY(float rotAngle)
	{
		m_pos = glm::vec3(glm::rotate(glm::mat4(1), rotAngle, glm::vec3(0, 1, 0)) * glm::vec4(m_pos, 1));

		m_up = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), rotAngle, glm::vec3(0, 1, 0)) * glm::vec4(m_up, 0))); ///??????????
		m_fwd = glm::normalize(-m_pos);
		m_right = glm::normalize(glm::cross(m_fwd, m_up));
		m_view = glm::lookAt(m_pos, m_pos + m_fwd, m_up);
	}
	void RotateAroundOriginX(float rotAngle)
	{
		m_pos = glm::vec3(glm::rotate(glm::mat4(1), rotAngle, m_right) * glm::vec4(m_pos, 1));

		m_up = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), rotAngle, m_right) * glm::vec4(m_up, 0))); ///??????????
		m_fwd = glm::normalize(-m_pos);
		m_right = glm::normalize(glm::cross(m_fwd, m_up));
		m_view = glm::lookAt(m_pos, m_pos + m_fwd, m_up);
	}
	
	void RotateAroundPointY(float rotAngle, glm::vec3 point)
	{
		m_pos = glm::vec3(glm::translate(glm::mat4(1), point)
						* glm::rotate(glm::mat4(1), rotAngle, glm::vec3(0, 1, 0))
					    * glm::translate(glm::mat4(1), -point)
						* glm::vec4(m_pos, 1));
	
		m_up = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), rotAngle, glm::vec3(0, 1, 0)) * glm::vec4(m_up, 1)));
		
		m_fwd = glm::normalize(point-m_pos);
		m_right = glm::normalize(glm::cross(m_fwd, m_up));
		m_view = glm::lookAt(m_pos, m_pos + m_fwd, m_up);
	}
	void RotateAroundPointX(float rotAngle, glm::vec3 point)
	{
		glm::vec3 oldPos = m_pos;
		m_pos = glm::vec3(glm::translate(glm::mat4(1), point)
						* glm::rotate(glm::mat4(1), rotAngle, m_right)
						* glm::translate(glm::mat4(1), -point)
						* glm::vec4(m_pos, 1));


		m_up = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), rotAngle, m_right) 
							* glm::vec4(m_up, 0)));
		m_fwd = glm::normalize(point - m_pos);
		m_right = glm::normalize(glm::cross(m_fwd, m_up));
		m_view = glm::lookAt(m_pos, m_pos + m_fwd, m_up);
	}
	glm::mat4 GetViewMatrix() { return m_view; }
	glm::vec3 GetPosition() { return m_pos; }
	void FixOZRotationYup()
	{
		m_view = glm::lookAt(m_pos, m_pos + m_fwd, glm::vec3(0,1,0));
		m_right = glm::normalize(glm::vec3(m_right.x, 0, m_right.z));
		m_up = glm::normalize(glm::cross(m_right, m_fwd));
	}
};