#include <stdbool.h>
#include <stdio.h>
#include "object.h"
#include "misc.h"
#include "match.h"
#include "noun.h"
#include "reach.h"

static int talk(const char* about, Object* to) {
    Object* topic = getTopic(about);
    if (topic == NULL) {
        printf("I don't understand what you want to talk about.\n");
        return 0;
    } else {
        printf("You hear %s say: '%s'\n", to->description, topic == to ? "I don't want to talk about myself." : topic->gossip);
        return 1;
    }
}

int executeTalk(void) {
    Object* to = actorHere();
    if (to != NULL) {
        return talk(params[0], to);
    } else {
        printf("There is no one here to talk to.\n");
        return 0;
    }
}

int executeTalkTo(void) {
    Object* to = actorHere();
    if (to != NULL) {
        if (to->health > 0) {
            return talk(params[0], to);
        } else {
            printf("There is no response from %s.\n", to->description);
            return 1;
        }
    }
    return 0;
}
