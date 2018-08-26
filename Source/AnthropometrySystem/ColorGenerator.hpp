	#pragma once
#include <include/glm.h>

class ColorGenerator
{
public:
	void SetReservedColors(std::vector<glm::uvec3> &reserved)
	{
		reservedColors = reserved;
	}

	glm::uvec3 getNextColor()
	{
		float step = 1;

		crtB++;
		if (crtB > 255)
		{
			crtG++;
			crtB = 0;
		}

		if (crtG > 255)
		{
			crtR++;
			crtG = 0;
		}

		if (crtR > 255)
		{
			doneSixteenMillion = true;
		}

		glm::uvec3 ret = glm::uvec3(crtR, crtG, crtB);
		for(auto rc : reservedColors)
			if (ret == rc)
				return getNextColor();
		
		return glm::uvec3(crtR, crtG, crtB);
	}
private:
	std::vector<glm::uvec3> reservedColors;
	GLubyte crtR = 0, crtG = 200, crtB = 1;
	bool doneSixteenMillion = false;
};

