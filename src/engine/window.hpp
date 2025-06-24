#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Engine {
class Window {
public:
  Window(int width, int height);
  void close();
  bool shouldWindowClose();
  void pollEvents();
  GLFWwindow *getWindow() const;
  bool didWindowResize() const;
  void resetWindowResized();
  static void windowResizeCallback(GLFWwindow *window, int width, int height);

private:
  GLFWwindow *m_window;
  int m_height;
  int m_width;
  bool m_windowResized;
};
} // namespace Engine
