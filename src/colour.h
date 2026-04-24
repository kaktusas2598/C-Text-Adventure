#ifndef COLOUR_H
#define COLOUR_H

// Global flag to enable/disable colors
extern bool colourEnabled;
 
// ANSI color codes (empty if color is disabled)
#define RESET       (colourEnabled ? "\033[0m" : "")
#define BOLD        (colourEnabled ? "\033[1m" : "")
#define RED         (colourEnabled ? "\033[31m" : "")
#define GREEN       (colourEnabled ? "\033[32m" : "")
#define YELLOW      (colourEnabled ? "\033[33m" : "")
#define BLUE        (colourEnabled ? "\033[34m" : "")
#define BRIGHT_RED  (colourEnabled ? "\033[91m" : "")
 
// Initialize color support (call once at program start)
void initColours();
// Check if terminal colours are supported
bool isTerminalColourSupported();

#endif