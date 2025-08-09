#pragma once
#include "renderer/renderer.hpp"
#include "renderer/window.hpp"

#include <entt/entity/registry.hpp>
#include <glm/fwd.hpp>

namespace Aether {
class RenderSystem {
public:
  explicit RenderSystem(Renderer::Renderer &renderer) : m_renderer(renderer) {
    m_renderable2Ds.reserve(1024);
  }

  void update(entt::registry &reg);
  void render(entt::registry &reg);
  void enqueueRenderable2D(entt::entity e);
  void dequeueRenderable2D(entt::entity e);

private:
  Renderer::Renderer &m_renderer;
  std::vector<entt::entity> m_renderable2Ds;
};
} // namespace Aether