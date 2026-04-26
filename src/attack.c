#include <stdbool.h>
#include <stdio.h>
#include "object.h"
#include "misc.h"
#include "match.h"
#include "noun.h"
#include "reach.h"
#include "damage.h"

static Object* targetHere() {
    Object* target = actorHere();
    if (target == NULL) {
        printf("There is no one here to attack.\n");
    }
    return target;
}

int executeAttack(void) {
    Object* target = *params[0] == '\0' ? targetHere() : reachableObject("who to attack", params[0]);

    if (target != NULL) {
        Object* weapon = *params[1] == '\0' ? getOptimalWeapon(player) : getPossession(player, "wield", params[1]);
        if (weapon != NULL) {
            dealDamage(player, weapon, target);
            return 1;
        }
    }
    return 0;
}