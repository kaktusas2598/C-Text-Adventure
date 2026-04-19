#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "match.h"
#include "misc.h"
#include "noun.h"

int executeLookAround(void) {
    if (isLit(player->location)) {
        printf("You are in %s.\n", player->location->description);
    } else {
        printf("It's too dark to see anything.\n");
    }
    listObjectsAtLocation(player->location);
    return 1;
}

int executeLook(void) {
    Object* obj = getVisible("What do you want to look at?", params[0]);
    switch (getDistance(player, obj)) {
        case distHereContained:
            printf("Hard to see, try to get it first.\n");
            return 0;
        case distOverthere:
            printf("Too far away, move closer.\n");
            return 0;
        case distNotHere:
            printf("You don't see any %s here.\n", params[0]);
            return 0;
        case distUnknownObject:
            // already handled in getVisible()
            return 0;
        default:
            printf("%s\n", obj->details);
            listObjectsAtLocation(obj);
            return 1;
    }
}

static void movePlayer(Object* passage) {
    printf("%s\n", passage->textGo);
    if (passage->destination != NULL) {
        player->location = passage->destination;
        printf("\n");
        executeLookAround();
    }
}

int executeGo(void) {
    Object* obj = getVisible("Where do you want to go?", params[0]);
    switch (getDistance(player, obj)) {
        case distOverthere:
            movePlayer(getPassage(player->location, obj));
            return 1;
        case distNotHere:
            printf("You don't see any %s here.\n", params[0]);
            return 0;
        case distUnknownObject:
            // already handled in getVisible()
            return 0;
        default:
            movePlayer(obj);
            return 1;
    }
}
