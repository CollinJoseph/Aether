#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Aether {
struct WindowProperties {
  int width, height;
};

class Window {
public:
  bool m_windowResized;

public:
  Window(int width, int height);
  void close();
  bool shouldWindowClose();
  void pollEvents();
  GLFWwindow *getWindow() const;
  WindowProperties getWindowProperties();
  static void windowResizeCallback(GLFWwindow *window, int width, int height);

private:
  GLFWwindow *m_window;
  int m_height;
  int m_width;
};
} // namespace Aether
