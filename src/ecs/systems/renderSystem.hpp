#pragma once
#include "config/config.hpp"
#include "renderer/renderer.hpp"
#include <entt/entity/registry.hpp>

namespace Aether {
class RenderSystem {
public:
  explicit RenderSystem(Renderer::Renderer &renderer) : m_renderer(renderer) {
    m_2dRenderables.reserve(Config::MAX_RENDERABLES);
    m_2dTransforms.reserve(Config::MAX_RENDERABLES);
  }

  void render(entt::registry &reg);
  void enqueueRenderable2D(entt::entity e);
  void dequeueRenderable2D(entt::entity e);

private:
  Renderer::Renderer &m_renderer;
  std::vector<entt::entity> m_2dRenderables;
  bool m_2dRenderablesChanged = false;
  std::vector<glm::mat4> m_2dTransforms;
};
} // namespace Aether