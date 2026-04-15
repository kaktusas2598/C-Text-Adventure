#include <stdbool.h>
#include <stdio.h>
#include "object.h"
#include "misc.h"

static int weightOfContents(Object* container) {
    int sum = 0;
    Object* obj;
    for (obj = objs; obj < endOfObjs; obj++) {
        if (obj->location == container) {
            if (isHolding(container, obj)) sum += obj->weight;
        }
    }
    return sum;
}

static void describeMove(Object* obj, Object* to) {
    if (to == player->location) {
        printf("You drop %s.\n", obj->description);
    } else if (to != player) {
        printf(to->health > 0 ? "You give %s to %s.\n" : "You put %s in %s.\n", obj->description, to->description);
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
    } else if (obj->weight > to->capacity) {
        printf("This is way too heavy.\n");
    } else if (obj->weight + weightOfContents(to) > to->capacity) {
        printf("That would become too heavy.\n");
    } else {
        describeMove(obj, to);
        obj->location = to;
    }
}