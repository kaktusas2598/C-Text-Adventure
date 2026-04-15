#include "noun.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "misc.h"

static bool objectHasTag(Object* obj, const char* noun) {
    if (noun != NULL && *noun != '\0') {
        const char** tag;
        for (tag = obj->tags; *tag != NULL; tag++) {
            if (strcmp(*tag, noun) == 0) return true;
        }
    }
    return false;
}

static Object ambigousNoun;

static Object* getObject(const char* noun, Object* from, Distance maxDistance) {
    Object* obj, *res = NULL;
    for (obj = objs; obj < endOfObjs; obj++) {
        if (objectHasTag(obj, noun) && getDistance(from, obj) <= maxDistance) {
            res = res == NULL ? obj : &ambigousNoun;
        }
    }
    return res;
}

Object* getVisible(const char* intention, const char* noun) {
    Object* obj = getObject(noun, player, distOverthere);
    if (obj == NULL) {
        if (getObject(noun, player, distNotHere) == NULL) {
            printf("I don't understand %s.\n", intention);
        } else {
            printf("You don't see any %s here.\n", noun);
        }
    } else if (obj == &ambigousNoun) {
        printf("Please specify which %s you mean.\n", noun);
        obj = NULL;
    }
        
    return obj;
}

Object* getPossession(Object* from, const char* verb, const char* noun) {
    Object* obj = NULL;
    if (from == NULL) {
        printf("I don't understand who you want to %s.\n", verb);
    } else if ((obj = getObject(noun, from, distHeldContained)) == NULL) {
        if (getObject(noun, player, distNotHere) == NULL) {
            printf("I don't undestand what you want to %s.\n", verb);
        } else if (from == player) {
            printf("You are not holding any %s.\n", noun);
        } else {
            printf("There appears to be no %s you can get from %s.\n", noun, from->description);
        }
    } else if (obj == &ambigousNoun) {
        printf("Please specify which %s you want to %s.\n", noun, verb);
        obj = NULL;
    } else if (obj == from) {
        printf("You should not be doing that to %s.\n", obj->description);
        obj = NULL;
    }

    return obj;
}