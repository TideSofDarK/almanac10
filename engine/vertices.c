#include "vertices.h"

static const float quad_vertices_and_tex_coords[] = {
	0.5f,  0.5f, 0.0f,		1.0f, 1.0f, /* top right */
	0.5f, -0.5f, 0.0f,		1.0f, 0.0f, /* bottom right */
	-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, /* bottom left */
	-0.5f,  0.5f, 0.0f,		0.0f, 1.0f  /* top left */
};

static const float sprite_center_vertices_and_tex_coords[] = {
	0.25f,  0.25f, 0.0f,		1.0f, 1.0f, /* top right */
	0.25f, -0.25f, 0.0f,		1.0f, 0.0f, /* bottom right */
	-0.25f, -0.25f, 0.0f,		0.0f, 0.0f, /* bottom left */
	-0.25f,  0.25f, 0.0f,		0.0f, 1.0f  /* top left */
};

static const float sprite_bottom_vertices_and_tex_coords[] = {
	0.25f,  0.5f, 0.0f,		1.0f, 1.0f, /* top right */
	0.25f, 0.0f, 0.0f,		1.0f, 0.0f, /* bottom right */
	-0.25f, 0.0f, 0.0f,		0.0f, 0.0f, /* bottom left */
	-0.25f,  0.5f, 0.0f,	0.0f, 1.0f  /* top left */
};

static const float render_texture_vertices_and_tex_coords[] = {
	1.0f,  1.0f, 0.0f,		1.0f, 1.0f, /* top right */
	1.0f, -1.0f, 0.0f,		1.0f, 0.0f, /* bottom right */
	-1.0f, -1.0f, 0.0f,		0.0f, 0.0f, /* bottom left */
	-1.0f,  1.0f, 0.0f,		0.0f, 1.0f  /* top left */
};

static const float cross_vertices[] = {
	0.5f,  0.0f,  0.0f,
	-0.5f,  0.0f,  0.0f,
	0.0f,  0.0f,  0.5f,
	0.0f,  0.0f,  -0.5f
};

const float* get_quad_vertices_and_tex_coords(int* buffer_size)
{
	*buffer_size = sizeof(quad_vertices_and_tex_coords);
	return quad_vertices_and_tex_coords;
}

const float* get_sprite_vertices_and_tex_coords(int* buffer_size, SpriteOriginMode origin_mode)
{
	switch (origin_mode)
	{
	//case OM_CENTER:
	case OM_BOTTOM:
		*buffer_size = sizeof(sprite_bottom_vertices_and_tex_coords);
		return sprite_bottom_vertices_and_tex_coords;
		break;
	default:
		*buffer_size = sizeof(sprite_center_vertices_and_tex_coords);
		return sprite_center_vertices_and_tex_coords;
		break;
	}
}

const float* get_render_texture_vertices_and_tex_coords(int* buffer_size)
{
	*buffer_size = sizeof(render_texture_vertices_and_tex_coords);
	return render_texture_vertices_and_tex_coords;
}

const float* get_cross_vertices(int* buffer_size)
{
	*buffer_size = sizeof(cross_vertices);
	return cross_vertices;
}