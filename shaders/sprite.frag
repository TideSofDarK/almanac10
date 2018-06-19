

#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform int sheetPosition;
uniform int sheetLength;
uniform vec4 solidColor;

void main()
{
	vec2 texCoord = TexCoord;

	float segSize = 1.0 / sheetLength;
	float t = segSize * sheetPosition + segSize * texCoord.x;
	vec2 new = vec2(t, texCoord.y);

	vec4 newColor = texture(texture1, new);

	if (newColor.a == 0.0)
	{
		discard;
	}

	if (solidColor.w == 1.0)
	{
		FragColor = solidColor;
		return;
	}

	// vec newColor = vec(1.0,0.0,1.0,1.0);
	

	FragColor = newColor;
}

