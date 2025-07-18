#include "include/Engine.hpp"

int main(){
    Engine newEngine(720, 1280, "main");
    while (newEngine.isRunning()) {
        newEngine.Update();
    }
    return 0;
}