#include "object.h"

typedef enum {
    distSelf,
    distHeld,
    distHeldContained,
    distLocation,
    distHere,
    distHereContained,
    distOverthere,
    distNotHere,
    distUnknownObject
} Distance;

extern bool isHolding(Object* container, Object* obj);
extern Object* getPassage(Object* from, Object* to);
extern Distance getDistance(Object* from, Object* to);
extern Object* actorHere(void);
extern int listObjectsAtLocation(Object* location);
