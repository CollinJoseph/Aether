#include "engine/engine.hpp"
#include "../renderer/window.hpp"
#include "ecs/componets/transform.hpp"
#include "ecs/systems/transformSystem.hpp"
#include "renderer/renderer.hpp"

namespace Aether {

Application::Application()
    : m_window(800, 600), m_renderer(m_window), m_renderSystem(m_renderer) {
  testing();
}

Application Application::createApplication() { return {Application()}; }

void Application::run() {
  mainLoop();
  cleanup();
}

void Application::mainLoop() {
  while (!m_window.shouldWindowClose()) {
    m_transformSystem.update(m_registry);
    m_renderSystem.update(m_registry);
    m_window.pollEvents();
    m_renderSystem.render(m_registry);
  }
}

void Application::testing() {
  auto e = m_registry.create();
  m_registry.emplace<ECS::Components::Position>(e, 1.0f, 1.0f);
  m_registry.emplace<ECS::Components::Rotation>(e, 0.0f);
  m_registry.emplace<ECS::Components::Scale>(e, 1.0f, 1.0f);
}

void Application::cleanup() { m_window.close(); }

Application::~Application() = default;
} // namespace Aether