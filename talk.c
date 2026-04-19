#include <stdbool.h>
#include <stdio.h>
#include "object.h"
#include "misc.h"
#include "match.h"
#include "noun.h"
#include "reach.h"

static void talk(const char* about, Object* to) {
    Object* topic = getTopic(about);
    if (topic == NULL) {
        printf("I don't understand what you want to talk about.\n");
    } else {
        printf("You hear %s say: '%s'\n", to->description, topic == to ? "I don't want to talk about myself." : topic->gossip);
    }
}

bool executeTalk(void) {
    Object* to = actorHere();
    if (to != NULL) {
        talk(params[0], to);
    } else {
        printf("There is no one here to talk to.\n");
    }
    return true;
}

bool executeTalkTo(void) {
    Object* to = actorHere();
    if (to != NULL) {
        if (to->health > 0) {
            talk(params[0], to);
        } else {
            printf("There is no response from %s.\n", to->description);
        }
    }
    return true;
}
