#ifndef WORLD_H
#define WORLD_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Object Object;


typedef struct {
    Object* objects;
    size_t count;
} World;

bool worldLoad(const char* filename);
void worldUnload(void);
const World* worldGet(void);
bool worldEvaluateCondition(const Object* object, bool* result);
bool worldLoadObjects(void);

#endif
