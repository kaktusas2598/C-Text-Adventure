#include <stdbool.h>
#include <stdio.h>
#include "object.h"
#include "misc.h"
#include "location.h"
#include "damage.h"

static Object* findBestWeaponAround(Object* actor, Object* weapon) {
    Object* obj;
    for (obj = objs; obj < endOfObjs; obj++) {
        if (isHolding(actor->location, obj) && obj->impact < weapon->impact)
            weapon = obj;
    }
    return weapon;
}

static void actorTakingTurn(Object* actor) {
    if (isHolding(player->location, actor) && actor->trust < 0) {
        Object* weapon = getOptimalWeapon(actor);
        Object* best = findBestWeaponAround(actor, weapon);
        if (weapon == best) {
            dealDamage(actor, weapon, player);
        } else {
            best->location = actor;
            printf("You see %s pick up %s.\n", actor->description, best->description);
        }
    }
}

static void depleteLight(Object* obj, int time) {
    if ((obj->light -= time) <= 0 && (isHolding(player, obj) || isHolding(player->location, obj))) {
        printf("You see %s go out.\n", obj->description);
    }
}

bool turn(int time) {
    if (time > 0) {
        bool originallyLit = isLit(player->location);
        Object* obj, *originalLocation = player->location;
        for (obj = objs; obj < endOfObjs; obj++) {
            if (validObject(obj) && obj->location != NULL) {
                if (obj->health > 0) actorTakingTurn(obj);
                if (obj->light > 0) depleteLight(obj, time);
            }
        }
        if (player->health <= 0) {
            printf("You have died!\n");
            player->location = heaven;
            player->health = 100;
            for (obj = objs; obj < endOfObjs; obj++) {
                if (obj->location == player) obj->location = field;
            }
        }
        if (originallyLit != isLit(player->location) || originalLocation != player->location) {
            executeLookAround();
        }
    }
    return time >= 0;
}

