#include "misc.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "object.h"

#include "colour.h"

bool isHolding(Object* container, Object* obj) {
    return validObject(obj) && obj->location == container;
}

bool isLit(Object* location) {
    Object* obj;
    if (validObject(location)) {
        if (location->light > 0) {
            return true;
        }
        for (obj = objs; obj < endOfObjs; obj++) {
            if (validObject(obj) && obj->light > 0 && (isHolding(location, obj) || isHolding(location, obj->location))) {
                return true;
            }
        }
    }
    return false;
}

static bool isNoticable(Object* obj) {
    return obj->location == player || isLit(obj) || isLit(obj->prospect) || isLit(player->location);
}

Object* getPassage(Object* from, Object* to) {
    if (from != NULL && to != NULL) {
        Object* obj;
        for (obj = objs; obj < endOfObjs; obj++) {
            if (isHolding(from, obj) && obj->prospect == to) {
                return obj;
            }
        }
    }

    return NULL;
}

Distance getDistance(Object* from, Object* to) {
    return to == NULL                               ? distUnknownObject :
            !validObject(to)                        ? distNotHere :
            to == from                              ? distSelf :
            isHolding(from, to)                     ? distHeld :
            !isNoticable(to)                        ? distNotHere :
            isHolding(to, from)                     ? distLocation :
            isHolding(from->location, to)           ? distHere :
            isHolding(from, to->location)           ? distHeldContained :
            isHolding(from->location, to->location) ? distHereContained :
            getPassage(from->location, to) != NULL  ? distOverthere :
                                                      distNotHere;
}

Object* actorHere(void) {
    Object* obj;
    for (obj = objs; obj < endOfObjs; obj++) {
        if (isHolding(player->location, obj) && obj != player && isNoticable(obj) && obj->health > 0) {
            return obj;
        }
    }

    return NULL;
}

int listObjectsAtLocation(Object* location) {
    int count = 0;
    Object* obj;
    for (obj = objs; obj < endOfObjs; obj++) {
        if (obj != player && isHolding(location, obj) && isNoticable(obj)) {
            if (count++ == 0) {
                printf("%s:\n", location->contents);
            }

            if (strcmp(obj->kind, "actor") == 0) {
                printf("%s\n", obj->description);
            } else if (strcmp(obj->kind, "container") == 0 || strcmp(obj->kind, "passage") == 0) {
                printf("%s%s%s\n", BLUE, obj->description, RESET);
            } else {
                printf("%s%s%s\n", GREEN, obj->description, RESET);
            }
        }
    }

    return count;
}
