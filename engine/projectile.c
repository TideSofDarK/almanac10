#include "projectile.h"

void construct_projectile(Projectile** _projectile, Sprite* sprite)
{
	*_projectile = malloc(sizeof(Projectile));
	Projectile* projectile = *_projectile;

	projectile->dead = 0;

	projectile->sprite = sprite;

	projectile->radius = PROJ_DEFAULT_RADIUS;
	projectile->lifetime = PROJ_DEFAULT_LIFETIME;
	projectile->speed = PROJ_DEFAULT_SPEED;

	glm_vec_zero(projectile->origin);
	glm_vec_one(projectile->direction);
	glm_vec_one(projectile->assumed_target);

    init_transform(&projectile->transform);
}

void destruct_projectile(Projectile** _projectile)
{
	Projectile* projectile = *_projectile;

	destruct_sprite(&projectile->sprite);
	projectile->sprite = NULL;

	free(*_projectile);
	*_projectile = NULL;
}

void launch_projectile(Projectile* projectile)
{
	projectile->begin = clock();

	glm_vec_copy(projectile->origin, projectile->transform.pos);

	/* Calculate assumed target (origin + direction * speed * lifetime) */
	glm_vec_copy(projectile->origin, projectile->assumed_target);
	glm_vec_muladds(projectile->direction, projectile->speed * projectile->lifetime, projectile->assumed_target);
}

void kill_projectile(Projectile* projectile)
{
	projectile->dead = 1;
	projectile->begin = 0;

	play_sprite_animation(projectile->sprite, ANIM_DEATH);
}

void update_projectile(Projectile* projectile)
{
	if (projectile->begin) /* Maybe it needs a better check? */
	{
		float projectile_time = (float)(clock() - projectile->begin) / CLOCKS_PER_SEC;
		float normalized = projectile_time / projectile->lifetime;

		if (normalized >= 1.0f)
		{
			kill_projectile(projectile);
			return;
		}
		else
		{
			glm_vec_lerp(projectile->origin, projectile->assumed_target, normalized, projectile->transform.pos);
		}
	}

	update_sprite(projectile->sprite);
}