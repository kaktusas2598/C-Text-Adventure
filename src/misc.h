#include "object.h"

typedef enum {
    distSelf,           // The object is the player
    distHeld,           // The object is being held by the player
    distHeldContained,  // The object is being held by the player and contains something (eg. a bag)
    distLocation,       // The object is the player's location
    distHere,           // The object is present at the player's location    
    distHereContained,  // Another object is present at the player's location and holding an object
    distOverthere,      // The object is at nearby location
    distNotHere,        // The object is(or appears to be) not here
    distUnknownObject   // The parser did not recognize the noun entered
} Distance;

extern bool isHolding(Object* container, Object* obj);
extern bool isLit(Object* location);
extern Object* getPassage(Object* from, Object* to);
extern Distance getDistance(Object* from, Object* to);
extern Object* actorHere(void);
extern int listObjectsAtLocation(Object* location);
