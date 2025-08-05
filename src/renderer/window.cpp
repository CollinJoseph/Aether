#define GLFW_INCLUDE_VULKAN
#include "window.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

namespace Aether {

Window::Window(int width, int height) {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  m_window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
  if (m_window == NULL) {
    std::cout << "Failed to create window" << std::endl;
  }
  glfwGetWindowSize(m_window, &m_width, &m_height);
  glfwSetWindowUserPointer(m_window, this);
  glfwSetFramebufferSizeCallback(m_window, windowResizeCallback);
}

GLFWwindow *Window::getWindow() const { return m_window; }

WindowProperties Window::getWindowProperties() {
  glfwGetFramebufferSize(m_window, &m_width, &m_height);
  return {m_width, m_height};
}

void Window::windowResizeCallback(GLFWwindow *window, int width, int height) {
  auto *windowPtr = static_cast<Window *>(glfwGetWindowUserPointer(window));
  windowPtr->m_windowResized = true;
}

void Window::close() {
  glfwDestroyWindow(m_window);
  glfwTerminate();
}

bool Window::shouldWindowClose() { return glfwWindowShouldClose(m_window); }

void Window::pollEvents() { glfwPollEvents(); }

} // namespace Aether
