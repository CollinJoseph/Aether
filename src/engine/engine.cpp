#include "engine.hpp"
#include "renderer/renderer.hpp"
#include "window.hpp"

namespace Engine {

Application::Application() : window(800, 600), renderer(window) {}

Application Application::createApplication() { return {Application()}; }

void Application::run() {
  mainLoop();
  cleanup();
}

void Application::mainLoop() {
  while (!window.shouldWindowClose()) {
    window.pollEvents();
    renderer.drawFrame();
  }
}

void Application::cleanup() { window.close(); }

} // namespace Engine
