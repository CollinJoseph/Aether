#pragma once
#include <memory>

namespace Renderer {
class Renderer;
}

namespace Engine {
class Window;

class Application {
public:
  static Application createApplication();
  ~Application();
  void run();

private:
  Application();
  std::unique_ptr<Window> m_window;
  std::unique_ptr<Renderer::Renderer> m_renderer;
  void mainLoop();
  void cleanup();
};
} // namespace Engine
