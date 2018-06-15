#include "creature.h"

#include <assimp/types.h>

#include "vector.h"
#include "script.h"
#include "util.h"

static CreatureData* precached_creatures = NULL;

int precache_creature(const char * script_name)
{
	/* TODO: actually precache stuff */
	vector_push_back(precached_creatures, parse_lua_creature(script_name));

	return (int)vector_size(precached_creatures) - 1;
}

CreatureData const * get_precached_creature_data(const char* name)
{
	char * script_name = NULL;
	asprintf(&script_name, "c_%s", name);

	for (size_t i = 0; i < vector_size(precached_creatures); i++)
	{
		if (strcmp(script_name, precached_creatures[i].script_name) == 0)
		{
			return &precached_creatures[i];
		}
	}

	int index = precache_creature(script_name);
	return &precached_creatures[index];
}

void free_precached_creatures()
{
	for (size_t i = 0; i < vector_size(precached_creatures); i++)
	{
        free(precached_creatures[i].script_name);
		free(precached_creatures[i].name);
		free(precached_creatures[i].sprite_sheet_folder);
	}

	vector_free(precached_creatures);
}

void construct_creature(Creature** _creature, CreatureData const * creature_data, vec3 pos)
{
	*_creature = malloc(sizeof(Creature));
	Creature* creature = *_creature;

    init_transform(&creature->transform);
	glm_vec_copy(pos, creature->transform.pos);
	glm_vec_copy(pos, creature->start_pos);

	creature->sprite = NULL;
	sprite_creature(&creature->sprite, creature_data->sprite_sheet_folder);

	creature->ai_state = AISTATE_ROAM;
	creature->attack_target = NULL;

	glm_vec_zero(creature->roam_pos);
	creature->roam_clock = -1;

	creature->data = creature_data;
	reset_creature_stats(creature);

	creature->begin = clock();

	creature->dead = 0;
}

void destruct_creature(Creature** _creature)
{
	Creature* creature = *_creature;
	
	destruct_sprite(&creature->sprite);

	free(*_creature);
	*_creature = NULL;
}

void reset_creature_stats(Creature* creature)
{
	assert(creature->data != NULL);
	creature->name = creature->data->name;
	creature->health = creature->max_health = creature->data->health;
	creature->mana = creature->max_mana = creature->data->mana;
	creature->attack_dice_count = creature->data->attack_dice_count;
	creature->attack_dice = creature->data->attack_dice;
	creature->attack_bonus = creature->data->attack_bonus;
	creature->movement_capability = creature->data->movement_capability;
}

void kill_creature(Creature* creature)
{
	creature->dead = 1;
	creature->ai_state = AISTATE_NONE;

	play_sprite_animation(creature->sprite, ANIM_DEATH);
}

void update_creature(Creature* creature)
{
	/* TODO: correct animation playback conditions */
	if (!creature->dead && creature->ai_state == AISTATE_ROAM && creature->movement_capability != MOVCAP_NONE)
	{
		if (creature->roam_clock < 0)
		{
			/* Find new roam position */
			float max_distance = 1.75f;
			float min_distance = 0.5f;
			vec3 pos;
			do
			{
				float distance = min_distance + ((float)rand() / (float)(RAND_MAX / (max_distance - min_distance)));
				float angle = (float)rand() / (float)(RAND_MAX / (M_PI * 2));

				/* TODO: don't forget about y! */
				float x = distance * cosf(angle);
				float y = creature->movement_capability == MOVCAP_FLY ? cosf(angle) / 2.0f : 0.0f;
				float z = distance * sinf(angle);

				glm_vec_add((vec3) {x, y, z}, creature->start_pos, pos);
			} while (glm_vec_distance(pos, creature->start_pos) >= max_distance);

			glm_vec_copy(creature->transform.pos, creature->roam_start_pos);
			glm_vec_add(creature->start_pos, pos, creature->roam_pos);

			creature->roam_clock = clock();
		}
		else
		{
			float roam_time = (float)(clock() - creature->roam_clock) / CLOCKS_PER_SEC;
			float normalized = (roam_time * 0.8f) / glm_vec_distance(creature->roam_start_pos, creature->roam_pos);
			
			if (normalized >= 1.0f) /* We're done; go look for new roaming position */
			{
				creature->roam_clock = -1;
			}
			else
			{
				vec3 t;
				glm_vec_sub(creature->roam_pos, creature->transform.pos, t);

				glm_vec3((vec3) {
					fmodf(360.0f - (glm_deg((atan2f(t[0], t[2]))) + 180.0f), 360.0f), 0, 0
				}, creature->transform.euler);

				glm_vec_lerp(creature->roam_start_pos, creature->roam_pos, normalized, creature->transform.pos);
			}
		}
	}

	update_sprite(creature->sprite);
}