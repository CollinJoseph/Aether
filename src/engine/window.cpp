#define GLFW_INCLUDE_VULKAN
#include "window.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

namespace Engine {

Window::Window(int width, int height) {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  m_window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
  if (m_window == NULL) {
    std::cout << "Failed to create window" << std::endl;
  }
  glfwGetWindowSize(m_window, &m_width, &m_height);
}

GLFWwindow *Window::getWindow() { return m_window; }

void Window::close() {
  glfwDestroyWindow(m_window);
  glfwTerminate();
}

bool Window::shouldWindowClose() { return glfwWindowShouldClose(m_window); }

void Window::pollEvents() { glfwPollEvents(); }

} // namespace Engine
