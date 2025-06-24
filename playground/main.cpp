#include <engine/engine.hpp>
#include <iostream>

int main() {
  std::cout << "Hello from the Sandbox app!" << std::endl;
  std::vector<VkDeviceMemory> deviceMemoryAllocations;

  Engine::Application engine = Engine::Application::createApplication();
  engine.run();

  return 0;
}
