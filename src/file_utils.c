#include "file_utils.h"

#include <stdio.h>

bool fileExists(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return false;
    }
    fclose(file);
    return true;
}
