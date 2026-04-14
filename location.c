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
        printf("I don't understand what you're trying to look at.\n");
    }
}

void executeGo(const char* noun) {
    Object* obj = getVisible("Where do you want to go?", noun);
    switch (getDistance(player, obj)) {
        case distOverthere:
            printf("OK.\n");
            player->location = obj;
            executeLook("around");
            break;
        case distNotHere:
            printf("You don't see any %s here.\n", noun);
            break;
        case distUnknownObject:
            // already handled in getVisible()
            break;
        default:
            if (obj->destination != NULL)
            {
                printf("OK.\n");
                player->location = obj->destination;
                executeLook("around");
            }
            else
            {
                printf("Can't get much closer than this.\n");
            }
            break;
    }
}
