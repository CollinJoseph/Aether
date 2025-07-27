#pragma once
#include "renderer/renderer.hpp"
#include "renderer/window.hpp"

#include <entt/entity/registry.hpp>
#include <glm/fwd.hpp>

namespace Aether {
class RenderSystem {
public:
  explicit RenderSystem(Renderer::Renderer &renderer) : m_renderer(renderer) {}

  void update(entt::registry &reg);
  void render(entt::registry &reg);

private:
  Renderer::Renderer &m_renderer;
  std::vector<glm::mat4> m_quadTransforms;
};
} // namespace Aether