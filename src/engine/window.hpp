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
  GLFWwindow *getWindow();

private:
  GLFWwindow *m_window;
  int m_height;
  int m_width;
};
} // namespace Engine
