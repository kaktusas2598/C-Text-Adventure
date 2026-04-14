#include <stdio.h>
#include "object.h"

static void describeMove(Object* obj, Object* to) {
    if (to == player->location) {
        printf("You drop %s.\n", obj->description);
    } else if (to != player) {
        printf(to == guard ? "You give %s to %s.\n" : "You put %s in %s.\n", obj->description, to->description);
    } else if (obj->location == player->location) {
        printf("You pick up %s.\n", obj->description);
    } else {
        printf("You get %s from %s.\n", obj->description, obj->location->description);
    }
}

void moveObject(Object* obj, Object* to) {
    if (obj == NULL) {
        // Handled by getVisible() or getPossesion()
    } else if (to == NULL) {
        printf("There is nobody here to give that to.\n");
    } else if (obj->location == NULL) {
        printf("This is way too heavy.\n");
    } else {
        describeMove(obj, to);
        obj->location = to;
    }
}