#include "engine.h"

int main(int argc, char* argv[]) {
    bool debugMode = false;

    for(int i=1;i<argc;i++){
        if (strcmp(argv[i], "--debugMode") == 0){
            debugMode = true;
        }
    }

    Engine* graphicsEngine = new Engine(de  bugMode);

    delete graphicsEngine;

    return 0;
}