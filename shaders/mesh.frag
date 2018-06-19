

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec4 solid_color;

void main()
{
	if (solid_color.a > 0)
	{
		FragColor = solid_color;	
	}
	else
	{
    	FragColor = texture(texture_diffuse1, TexCoords);		
	}
}

