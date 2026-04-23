#include <stdbool.h>
#include <stdio.h>
#include "object.h"
#include "match.h"
#include "reach.h"
#include "toggle.h"

int executeTurnOn(void) {
    Object* obj = reachableObject("what to turn on", params[0]);
    if (obj != NULL) {
        if (obj->togglesTo != NULL) {
            toggleObject(obj);
        } else {
            printf("You cannot turn that on.\n");
        }
        return 1;
    }

    return 0;
}

int executeTurnOff(void) {
    Object* obj = reachableObject("what to turn off", params[0]);
    if (obj != NULL) {
        if (obj->togglesTo != NULL) {
            toggleObject(obj);
        } else {
            printf("You cannot turn that off.\n");
        }
        return 1;
    }

    return 0;
}