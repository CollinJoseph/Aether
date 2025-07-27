#pragma once
#include "../renderer/window.hpp"
#include "ecs/systems/renderSystem.hpp"
#include "ecs/systems/transformSystem.hpp"
#include "renderer/renderer.hpp"

#include <entt/entt.hpp>

namespace Engine {
class Application {
public:
  static Application createApplication();
  ~Application();
  void run();

private:
  Window m_window;
  Renderer::Renderer m_renderer;
  entt::registry m_registry;
  ECS::Systems::TransformSystem m_transformSystem;
  Aether::RenderSystem m_renderSystem;

private:
  Application();
  void mainLoop();
  void testing();
  void cleanup();
};
} // namespace Engine
