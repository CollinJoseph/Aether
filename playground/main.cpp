#include <engine/engine.hpp>
#include <iostream>

int main() {
  std::cout << "Hello from the Sandbox app!" << std::endl;

  Aether::Application engine = Aether::Application::createApplication();
  engine.run();

  return 0;
}
