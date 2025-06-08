#include <iostream>
#include <engine/engine.hpp>

int main()
{
    std::cout << "Hello from the Sandbox app!" << std::endl;

    Engine::Engine engine;

    engine.Init();
    engine.Run();
    engine.Shutdown();

    return 0;
}