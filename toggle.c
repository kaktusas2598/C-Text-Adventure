#include <stdbool.h>
#include <stdio.h>
#include "object.h"
#include "misc.h"
#include "location.h"

static void swapStates(const char* verb, Object* current, Object* other) {
    Object* tmp = current->location;
    current->location = other->location;
    other->location = tmp;
    if (verb != NULL) printf("You %s %s.\n", verb, current->description);
}


void cannotBeOpened(Object* obj)    { (void)obj; printf("That cannot be opened.\n");    }
void cannotBeClosed(Object* obj)    { (void)obj; printf("That cannot be closed.\n");    }
void cannotBeLocked(Object* obj)    { (void)obj; printf("That cannot be locked.\n");    }
void cannotBeUnlocked(Object* obj)  { (void)obj; printf("That cannot be unlocked.\n");  }
void isAlreadyOpen(Object* obj)     { (void)obj; printf("That is already open.\n");     }
void isAlreadyClosed(Object* obj)   { (void)obj; printf("That is already closed.\n");   }
void isAlreadyLocked(Object* obj)   { (void)obj; printf("That is already locked.\n");   }
void isAlreadyUnlocked(Object* obj) { (void)obj; printf("That is already unlocked.\n"); }
void isStillOpen(Object* obj)       { (void)obj; printf("That is still open.\n");       }
void isStillLocked(Object* obj)     { (void)obj; printf("That is locked.\n");           }

static bool playerHasKeyFor(Object* lockedObject) {
    return lockedObject->key != NULL && lockedObject->key->location == player;
}

static void syncMirror(Object* obj) {
    if (obj->mirrorsTo != NULL && obj->mirrorsTo->togglesTo != NULL) {
        swapStates(NULL, obj->mirrorsTo, obj->mirrorsTo->togglesTo);
    }
}

void openViaToggle(Object* obj) {
    if (obj == NULL || obj->togglesTo == NULL) {
        cannotBeOpened(obj);
        return;
    }
    syncMirror(obj);
    swapStates("open", obj, obj->togglesTo);
}

void closeViaToggle(Object* obj) {
    if (obj == NULL || obj->togglesTo == NULL) {
        cannotBeClosed(obj);
        return;
    }
    syncMirror(obj);
    swapStates("close", obj, obj->togglesTo);
}

void lockViaToggle(Object* obj) {
    if (obj == NULL || obj->locksTo == NULL) {
        cannotBeLocked(obj);
        return;
    }
    if (!playerHasKeyFor(obj)) {
        printf("You don't have the key for that.\n");
        return;
    }
    swapStates("lock", obj, obj->locksTo);
}

void unlockViaToggle(Object* obj) {
    if (obj == NULL || obj->locksTo == NULL) {
        cannotBeUnlocked(obj);
        return;
    }
    if (!playerHasKeyFor(obj)) {
        printf("You don't have the key for that.\n");
        return;
    }
    swapStates("unlock", obj, obj->locksTo);
}

void toggleObject(Object* obj) {
    bool oldLit;
    Object* other;

    if (obj == NULL || obj->togglesTo == NULL) {
        printf("You cannot toggle that.\n");
        return;
    }

    other = obj->togglesTo;
    oldLit = isLit(player->location);

    swapStates(obj->light > 0 ? "turn off" : "turn on", obj, other);
    if (oldLit != isLit(player->location)) {
        printf("\n");
        executeLookAround();
    }
}