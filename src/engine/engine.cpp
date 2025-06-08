#include "engine/engine.hpp"
#include <iostream>

namespace Engine {

Engine::Engine() {
}

void Engine::Init() {
    std::cout << "Engine Init\n";
}

void Engine::Run() {
    std::cout << "Engine Run\n";
}

void Engine::Shutdown() {
    std::cout << "Shutting down\n";
}

}