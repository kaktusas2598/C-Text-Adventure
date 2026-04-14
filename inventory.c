#include <stdio.h>

#include "object.h"
#include "misc.h"
#include "noun.h"
#include "move.h"

void executeGet(const char* noun) {

    Object* obj = getVisible("what you want to get", noun);
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
            if (obj->location == guard) {
                printf("You should ask %s nicely.\n", obj->location->description);
            } else {
                moveObject(obj, player);
            }
            break;
    }
}

void executeDrop(const char* noun) {
    moveObject(getPossession(player, "drop", noun), player->location);
}

void executeAsk(const char* noun) {
    moveObject(getPossession(actorHere(), "ask", noun), player);
}

void executeGive(const char* noun) {
    moveObject(getPossession(player, "drop", noun), actorHere());
}

void executeInventory(void) {
    if (listObjectsAtLocation(player) == 0) {
        printf("You are poor as heck, your pockets are empty.\n");
    }
}
