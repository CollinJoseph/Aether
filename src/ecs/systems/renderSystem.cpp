#include "renderSystem.hpp"

#include "ecs/componets/renderable2D.hpp"
#include "ecs/componets/transform.hpp"
#include <glm/gtx/matrix_decompose.hpp>

namespace Aether {

void RenderSystem::update(entt::registry &reg) {
  const auto view =
      reg.view<ECS::Components::Transform2D, ECS::Components::Renderable2D>();
}

void RenderSystem::render(entt::registry &reg) {
  const auto view =
      reg.view<ECS::Components::Transform2D, ECS::Components::Renderable2D>();
  m_renderer.drawFrame();
}

void RenderSystem::enqueueRenderable2D(entt::entity e) {
  std::cout << static_cast<uint32_t>(e) << std::endl;
  m_renderable2Ds.push_back(e);
}

void RenderSystem::dequeueRenderable2D(entt::entity e) {
  std::cout << static_cast<uint32_t>(e) << std::endl;
  auto it = std::find(m_renderable2Ds.begin(), m_renderable2Ds.end(), e);
  if (it != m_renderable2Ds.end()) {
    *it = m_renderable2Ds.back();
    m_renderable2Ds.pop_back();
  }
}
} // namespace Aether