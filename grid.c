#include "grid.h"

#include "vertices.h"
#include "renderer.h"
#include "game.h"

const unsigned int GRID_SIZE = 20;

Grid grid;

void construct_grid()
{
	construct_shader(&grid.shader, "grid.vert", "grid.frag", NULL);

	glm_mat4_copy(GLM_MAT4_IDENTITY, grid.model);
	glm_translate_y(grid.model, 0.0f);
	glm_translate_x(grid.model, 0.5f * GRID_SIZE / -2.0f);
	glm_translate_z(grid.model, 0.5f * GRID_SIZE / -2.0f);
	//glm_scale(grid->model, (vec3) { 2.5, 2.5, 2.5 });

	int buffer_size = 0;
	const float* vertices = get_cross_vertices(&buffer_size);

	glGenVertexArrays(1, &grid.VAO);
	glBindVertexArray(grid.VAO);

	glGenBuffers(1, &grid.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, grid.VBO);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void destruct_grid()
{
	destruct_shader(&grid.shader);
}

int is_grid_constructed()
{
	return grid.VBO > 0;
}

void draw_grid()
{
	Camera camera = active_camera();

	//glDisable(GL_DEPTH_TEST);

	glBindVertexArray(grid.VAO);
	use_shader(grid.shader);
	set_uniform_mat4(grid.shader, "model", grid.model);
	set_uniform_mat4(grid.shader, "view", camera.view);
	set_uniform_mat4(grid.shader, "projection", camera.projection);

	glLineWidth(0.5f);
	for (unsigned int x = 0; x < GRID_SIZE; x++)
	{
		for (unsigned int z = 0; z < GRID_SIZE; z++)
		{
			mat4 model;
			glm_mat4_copy(grid.model, model);
			glm_translate_x(model, 0.5f * x);
			glm_translate_z(model, 0.5f * z);
			set_uniform_mat4(grid.shader, "model", model);

			glDrawArrays(GL_LINES, 0, 4);
		}
	}

	glBindVertexArray(0);

	//glEnable(GL_DEPTH_TEST);
}