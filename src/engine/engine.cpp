#include "engine/engine.hpp"
#include "../renderer/window.hpp"
#include "ecs/componets/renderable2D.hpp"
#include "ecs/componets/transform.hpp"
#include "ecs/systems/transformSystem.hpp"
#include "renderer/renderer.hpp"

namespace Aether {

Application::Application()
    : m_window(800, 600), m_renderer(m_window), m_renderSystem(m_renderer) {
  m_registry.on_destroy<ECS::Components::Renderable2D>()
      .connect<&RenderSystem::dequeueRenderable2D>(m_renderSystem);
  m_registry.on_construct<ECS::Components::Renderable2D>()
      .connect<&RenderSystem::enqueueRenderable2D>(m_renderSystem);
  testing();
}

Application Application::createApplication() { return {Application()}; }

void Application::run() {
  mainLoop();
  cleanup();
}

void Application::mainLoop() {
  while (!m_window.shouldWindowClose()) {
    m_window.pollEvents();
    if (random() % 100 > 90) {
      testing();
    }
    m_transformSystem.update(m_registry);
    m_renderSystem.render(m_registry);
  }
}

void Application::testing() {
  auto e = m_registry.create();
  m_registry.emplace<ECS::Components::Position>(e, rand() % 2000,
                                                rand() % 1000);
  m_registry.emplace<ECS::Components::Rotation>(e, rand() % 360);
  m_registry.emplace<ECS::Components::Scale>(e, rand() % 100, rand() % 100);
  m_registry.emplace<ECS::Components::Renderable2D>(e);
}

void Application::cleanup() {
  auto view = m_registry.view<ECS::Components::Renderable2D>();
  m_window.close();
}

Application::~Application() = default;
} // namespace Aether