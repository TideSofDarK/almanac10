

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 pos;
uniform float aspectRatio;
uniform int sheetInvert;

void main()
{
	vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
	vec3 up = vec3(view[0][1], view[1][1], view[2][1]);
	// vec3 forward = vec3(view[0][2], view[1][2], view[2][2]);
	
	vec3 vertexPosition = pos + (right * aPos.x * aspectRatio) + (aPos.y * up);
	gl_Position = projection * view * vec4(vertexPosition, 1.0f);

	TexCoord = vec2(abs(aTexCoord.x - sheetInvert), aTexCoord.y);
}

