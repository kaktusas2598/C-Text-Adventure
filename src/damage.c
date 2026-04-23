#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "object.h"
#include "misc.h"

static void describeAttack(Object* attacker, Object* target, Object* weapon) {
    const char* weaponDescription = weapon == attacker ? "bare hands" : weapon->description;
    if (attacker == player) {
        printf("You hit %s with %s.\n", target->description, weaponDescription);
    } else if (target == player) {
        printf("You are hit by %s with %s.\n", attacker->description, weaponDescription);
    } else {
        printf("You see %s hit %s with %s.\n", attacker->description, target->description, weaponDescription);
    }
}

static void describeDeath(Object* target) {
    if (target == player) {
        printf("You die.\n");
    } else {
        printf("You see %s die.\n", target->description);
    }
}

void dealDamage(Object* attacker, Object* weapon, Object* target) {
    int damage = (rand() % 6) * weapon->impact * attacker->health / 100;
    if (damage < 0) {
        if (target->health > 0) {
            describeAttack(attacker, target, weapon);
            target->health += damage;
            if (target->health <= 0) {
                target->health = 0;
                describeDeath(target);
            }
            if (attacker == player) {
                target->trust--;
            }
        } else if (attacker == player) {
            printf("That will have little effect; %s is already dead.\n", target->description);
        }
    } else if (attacker == player) {
        printf("You try to hit %s with %s, but you miss.\n",
            target->description,
            weapon == attacker ? "your bare hands" : weapon->description);
    }
}

Object* getOptimalWeapon(Object* attacker) {
    Object* obj, *weapon = attacker;
    for (obj = objs; obj < endOfObjs; obj++) {
        if (isHolding(attacker, obj) && obj->impact < weapon->impact)
            weapon = obj;
    }
    return weapon;
}