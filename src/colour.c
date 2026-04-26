#include "colour.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

bool colourEnabled = false;

// TODO: Add windows support? Not sure if I want it
bool isTerminalColourSupported() {
    // Set if user has not disabled colours by using NO_COLOR env
    if (getenv("NO_COLOR") != NULL) return false;

    // Check if TTY
    if (!isatty(fileno(stdout))) return false;

    // Check terminal type
    const char* term = getenv("TERM");
    if (!term) return false; // Unlikely not to be set in modern shell
    if (strcmp(term, "dumb") == 0) return false; // Dumb terminal
    return true; // Assume colour is enabled by this point
}

void initColours() {
    colourEnabled = isTerminalColourSupported();
};