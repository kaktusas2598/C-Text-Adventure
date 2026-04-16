#include <stdbool.h>
#include <stdio.h>

#include "object.h"
#include "misc.h"
#include "match.h"
#include "noun.h"
#include "move.h"

bool executeGet(void) {
    Object* obj = getVisible("what you want to get", params[0]);
    switch (getDistance(player, obj)) {
        case distSelf:
            printf("You should not do that to yourself.\n");
            break;
        case distHeld:
            printf("You already have %s in your bag.\n", obj->description);
            break;
        case distOverthere:
            printf("Too far away, move closer.\n");
            break;
        case distUnknownObject:
            // Already handled in getVisible()
            break;
        default:
            if (obj->location != NULL && obj->location->health > 0) {
                printf("You should ask %s nicely.\n", obj->location->description);
            } else {
                moveObject(obj, player);
            }
            break;
    }
    return true;
}

bool executeDrop(void) {
    moveObject(getPossession(player, "drop", params[0]), player->location);
    return true;
}

bool executeAsk(void) {
    moveObject(getPossession(actorHere(), "ask", params[0]), player);
    return true;
}

bool executeGive(void) {
    moveObject(getPossession(player, "give", params[0]), actorHere());
    return true;
}

bool executeInventory(void) {
    if (listObjectsAtLocation(player) == 0) {
        printf("You are poor as heck, your pockets are empty.\n");
    }
    return true;
}
