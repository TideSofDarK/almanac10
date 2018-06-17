#include "party.h"

Party create_random_party() {
    Party party;

    init_transform(&party.transform);

    for (int i = 0; i < 4; ++i) {
        party.heroes[i].name = "Alexis";
        party.heroes[i].class = C_ACOLYTE;
        party.heroes[i].stats.health = party.heroes[i].stats.max_health = 200;
        party.heroes[i].stats.mana = party.heroes[i].stats.max_mana = 100;
        party.heroes[i].stats.attack_dice_count = 2;
        party.heroes[i].stats.attack_dice = 4;
        party.heroes[i].stats.attack_bonus = 1;
        party.heroes[i].stats.status = HS_NONE;
    }

    return party;
}
