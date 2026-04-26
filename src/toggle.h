#ifndef TOGGLE_H
#define TOGGLE_H

// Forward declaration of Object struct to avoid circular dependency with object.h
typedef struct Object Object;

extern void cannotBeOpened(Object* obj);
extern void cannotBeClosed(Object* obj);
extern void cannotBeLocked(Object* obj);
extern void cannotBeUnlocked(Object* obj);

extern void isAlreadyOpen(Object* obj);
extern void isAlreadyClosed(Object* obj);
extern void isAlreadyLocked(Object* obj);
extern void isAlreadyUnlocked(Object* obj);

extern void isStillOpen(Object* obj);
extern void isStillLocked(Object* obj);

extern void openViaToggle(Object* obj);
extern void closeViaToggle(Object* obj);
extern void lockViaToggle(Object* obj);
extern void unlockViaToggle(Object* obj);

// For e.g.: Lamps: 2 objects one with light and other without with togglesTo attribute
extern void toggleObject(Object* obj);

#endif