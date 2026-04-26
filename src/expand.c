#include <stdbool.h>
#include <string.h>
#include "match.h"

// Structure used to hold shorthand commands and their expanded forms
// e.g., "n" for "north", "s" for "south", etc.
typedef struct {
    const char* abbreviated;
    const char* expanded;
} Shorthand;

static int minimum(int x, int y) {
    return (x < y) ? x : y;
}

char* expand(char* input, int bufferSize) {
    static const Shorthand shorthands[] = {
        {"n", "go north"},
        {"s", "go south"},
        {"e", "go east"},
        {"w", "go west"},
        {"q", "quit"},
        {"inv", "inventory"},
        {"x A", "examine "},
        {"A", NULL}
    };  
    const Shorthand* sh;
    for (sh = shorthands; !matchCommand(input, sh->abbreviated); sh++);
    if (sh->expanded != NULL) {
        const char* moreInput = *paramByLetter('A');
        int lengthExpanded = strlen(sh->expanded);
        memmove(input + lengthExpanded, moreInput, minimum(strlen(moreInput) + 1, bufferSize - lengthExpanded - 1));
        strncpy(input, sh->expanded, lengthExpanded);
    }

    return input;
}