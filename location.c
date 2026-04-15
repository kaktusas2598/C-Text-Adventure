#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "misc.h"
#include "noun.h"

void executeLook(const char* noun) {
    if (noun != NULL && strcmp(noun, "around") == 0) {
        printf("You are in %s.\n", player->location->description);
        listObjectsAtLocation(player->location);
    } else {
        Object* obj = getVisible("What do you want to look at?", noun);
        switch (getDistance(player, obj)) {
            case distHereContained:
                printf("Hard to see, try to get it first.\n");
                break;
            case distOverthere:
                printf("Too far away, move closer.\n");
                break;
            case distNotHere:
                printf("You don't see any %s here.\n", noun);
                break;
            case distUnknownObject:
                // already handled in getVisible()
                break;
            default:
                printf("%s\n", obj->details);
                listObjectsAtLocation(obj);
                break;
        }
    }
}

static void movePlayer(Object* passage) {
    printf("%s\n", passage->textGo);
    if (passage->destination != NULL) {
        player->location = passage->destination;
        printf("\n");
        executeLook("around");
    }
}

void executeGo(const char* noun) {
    Object* obj = getVisible("Where do you want to go?", noun);
    switch (getDistance(player, obj)) {
        case distOverthere:
            movePlayer(obj);
            break;
        case distNotHere:
            printf("You don't see any %s here.\n", noun);
            break;
        case distUnknownObject:
            // already handled in getVisible()
            break;
        default:
            movePlayer(obj);
            break;
    }
}
