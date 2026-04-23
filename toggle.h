#ifndef TOGGLE_H
#define TOGGLE_H

// Forward declaration of Object struct to avoid circular dependency with object.h
typedef struct Object Object;

extern void cannotBeOpened(void);
extern void cannotBeClosed(void);
extern void cannotBeLocked(void);
extern void cannotBeUnlocked(void);

extern void isAlreadyOpen(void);
extern void isAlreadyClosed(void);
extern void isAlreadyLocked(void);
extern void isAlreadyUnlocked(void);

extern void isStillOpen(void);
extern void isStillLocked(void);

extern void toggleDoorToBackroom(void);
extern void toggleDoorToCave(void);
extern void toggleBox(void);
extern void toggleBoxLock(void);

// For e.g.: Lamps: 2 objects one with light and other without with togglesTo attribute
extern void toggleObject(Object* obj);

#endif