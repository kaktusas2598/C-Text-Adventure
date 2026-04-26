#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "expand.h"
#include "object.h"
#include "parsexec.h"
#include "turn.h"
#include "world.h"
#include "colour.h"
#include "error.h"

static char input[100] = "look around";

static bool getFromFP(FILE* fp) {
    bool ok = fgets(input, sizeof input, fp) != NULL;
    if (ok) input[strcspn(input, "\n")] = '\0';
    return ok;
}

static bool getInput(const char* filename) {
    static FILE* fp = NULL;
    bool ok;
    if (fp == NULL) {
        if (filename != NULL) fp = fopen(filename, "rt");
        if (fp == NULL) fp = stdin;
    } else if (fp == stdin && filename != NULL) {
        FILE* out = fopen(filename, "at");
        if (out != NULL) {
            fprintf(out, "%s\n", input);
            fclose(out);
        }
    }

    printf("\n--> ");
    ok = getFromFP(fp);
    if (fp != stdin) {
        if (ok) {
            printf("%s\n", input);
        } else {
            fclose(fp);
            ok = getFromFP(fp = stdin);
        }
    }
    return ok;
}

static bool processInput(char* ptr, int size) {
    return turn(parseAndExecute(expand(ptr, size)));
}

int main(int argc, char* argv[]) {
    (void)argc;

    initColours();

    const char* worldFile = "world.lua";
    const char* saveFile = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-g") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Usage: %s [-g gameFile.lua]\n", argv[0]);
            }
            worldFile = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Usage: %s [-s saveFile.txt]\n", argv[0]);
                return 1;
            }
            saveFile = argv[++i];
        }
    }

    if (!worldLoad(worldFile)) {
        fprintf(stderr, "%sFailed to load %s: %s%s\n", RED, worldFile, getLastError(), RESET);
        return 1;
    }
    if (!worldLoadObjects()) {
        fprintf(stderr, "%sFailed to build runtime objects from %s: %s%s\n", RED, worldFile, getLastError(), RESET);
        worldUnload();
        return 1;
    }

    printf("Welcome to Text Adventure!\n");
    while(processInput(input, sizeof input) && getInput(saveFile));
    printf("\nGood Bye!\n");
    worldUnload();
    return 0;
}
