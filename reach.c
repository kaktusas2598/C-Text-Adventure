#include <stdbool.h>
#include <stdio.h>
#include "object.h"
#include "misc.h"
#include "noun.h"

Object* reachableObject(const char* intention, const char* noun) {
    Object* obj = getVisible(intention, noun);
    switch (getDistance(player, obj)) {
        case distSelf:
            printf("You should not be doing that to yourself.\n");
            break;
        case distHeldContained:
        case distHereContained:
            printf("You would have to get it from %s first.\n", obj->location->description);
            break;
        case distOverthere:
            printf("That is too far away.\n");
            break;
        case distNotHere:
        case distUnknownObject:
            // Already handled by getVisible.
             break;
        default:
            return obj;
    }

    return NULL;
}