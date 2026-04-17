#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "match.h"
#include "misc.h"
#include "noun.h"

bool executeLookAround(void) {
    if (isLit(player->location)) {
        printf("You are in %s.\n", player->location->description);
    } else {
        printf("It's too dark to see anything.\n");
    }
    listObjectsAtLocation(player->location);
    return true;
}

bool executeLook(void) {
    Object* obj = getVisible("What do you want to look at?", params[0]);
    switch (getDistance(player, obj)) {
        case distHereContained:
            printf("Hard to see, try to get it first.\n");
            break;
        case distOverthere:
            printf("Too far away, move closer.\n");
            break;
        case distNotHere:
            printf("You don't see any %s here.\n", params[0]);
            break;
        case distUnknownObject:
            // already handled in getVisible()
            break;
        default:
            printf("%s\n", obj->details);
            listObjectsAtLocation(obj);
            break;
    }
    return true;
}

static void movePlayer(Object* passage) {
    printf("%s\n", passage->textGo);
    if (passage->destination != NULL) {
        player->location = passage->destination;
        printf("\n");
        executeLookAround();
    }
}

bool executeGo(void) {
    Object* obj = getVisible("Where do you want to go?", params[0]);
    switch (getDistance(player, obj)) {
        case distOverthere:
            movePlayer(getPassage(player->location, obj));
            break;
        case distNotHere:
            printf("You don't see any %s here.\n", params[0]);
            break;
        case distUnknownObject:
            // already handled in getVisible()
            break;
        default:
            movePlayer(obj);
            break;
    }
    return true;
}
