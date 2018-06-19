

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D depth0;
uniform sampler2D depth1;
uniform float scale0;
uniform float scale1;

void main()
{
    ivec2 texcoord0 = ivec2(floor(gl_FragCoord.xy * scale0));
	ivec2 texcoord1 = ivec2(floor(gl_FragCoord.xy * scale1));

    float d0 = texelFetch(depth0, texcoord0, 0).r;
    float d1 = texelFetch(depth1, texcoord1, 0).r;

    if (d0 < d1)
	{
        FragColor = texelFetch(texture0, texcoord0, 0);
    }
	else
	{
        FragColor = texelFetch(texture1, texcoord1, 0);
    }
}

