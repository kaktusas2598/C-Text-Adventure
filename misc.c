#include "misc.h"
#include <stdio.h>
#include <stdbool.h>
#include "object.h"

bool isHolding(Object* container, Object* obj) {
    return obj != NULL && obj->location == container;
}

Object* getPassage(Object* from, Object* to) {
    if (from != NULL && to != NULL) {
        Object* obj;
        for (obj = objs; obj < endOfObjs; obj++) {
            if (obj->location == from && obj->destination == to) {
                return obj;
            }
        }
        
    }

    return NULL;
}

Distance getDistance(Object* from, Object* to) {
    return to == NULL                               ? distUnknownObject :
            to == from                              ? distSelf :
            isHolding(from, to)                     ? distHeld :
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
        if (isHolding(player->location, obj) && obj == guard) {
            return obj;
        }
    }

    return NULL;
}

int listObjectsAtLocation(Object* location) {
    int count = 0;
    Object* obj;
    for (obj = objs; obj < endOfObjs; obj++) {
        if (obj != player && isHolding(location, obj)) {
            if (count++ == 0) {
                printf("You see:\n");
            }
            printf("%s\n", obj->description);
        }
    }
}
