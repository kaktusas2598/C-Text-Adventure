#include "misc.h"
#include <stdio.h>

int listObjectsAtLocation(Object* location) {
    int count = 0;
    Object* obj;
    for (obj = objs; obj < endOfObjs; obj++) {
        if (obj != player && obj->location == location) {
            if (count++ == 0) {
                printf("You see:\n");
            }
            printf("%s\n", obj->description);
        }
    }
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

Object* actorHere(void) {
    Object* obj;
    for (obj = objs; obj < endOfObjs; obj++) {
        if (obj->location == player->location && obj == guard) {
            return obj;
        }
    }

    return NULL;
}