#pragma once
#include "renderer/renderer.hpp"
#include "window.hpp"

namespace Engine {
class Application {
public:
  static Application createApplication();
  void run();

private:
  Application();
  Window window;
  Renderer::Renderer renderer;
  void mainLoop();
  void cleanup();
};
} // namespace Engine
