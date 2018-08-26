#version 330

// Output data from fragment shader
 out vec4 colorOut;

uniform int width, height;


uniform sampler2D fullscreenTex;
uniform int solidColor;
uniform vec3 inColor;

in vec2 texCoords;

vec2 TCS;
vec2 CalcTexCoord(float offsetx, float offsety)
{
 //  return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(2732, 800);
	return (gl_FragCoord.xy + vec2(offsetx, offsety)) / vec2(width, height);
}

//uniform int solid_color;

void main()
{
//	return;
	if(solidColor == 0)
	{
		TCS = CalcTexCoord(0,0);

		colorOut = texture2D(fullscreenTex, texCoords);
		if(colorOut.a < 0.5)
			discard;
	}
	else
	{
		colorOut = vec4(inColor,1);
	}
	// colorOut = vec3(1,1,1);
}
