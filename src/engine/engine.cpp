#include "engine/engine.hpp"
#include "renderer/renderer.hpp"
#include "window.hpp"

namespace Engine {

Application::Application()
    : m_window(std::make_unique<Window>(800, 600)),
      m_renderer(std::make_unique<Renderer::Renderer>(*m_window)) {}

Application Application::createApplication() { return {Application()}; }

void Application::run() {
  mainLoop();
  cleanup();
}

void Application::mainLoop() {
  while (!m_window->shouldWindowClose()) {
    m_window->pollEvents();
    m_renderer->drawFrame();
  }
}

void Application::cleanup() { m_window->close(); }

Application::~Application() {}
} // namespace Engine