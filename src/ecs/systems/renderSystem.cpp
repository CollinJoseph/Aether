#include "renderSystem.hpp"

#include "ecs/componets/renderable2D.hpp"
#include "ecs/componets/transform.hpp"
#include <glm/gtx/matrix_decompose.hpp>

namespace Aether {

void RenderSystem::render(entt::registry &reg) {
  if (m_2dRenderablesChanged) {
    m_2dTransforms.clear();
    m_2dRenderablesChanged = false;
    std::transform(m_2dRenderables.begin(), m_2dRenderables.end(),
                   std::back_inserter(m_2dTransforms), [&](entt::entity e) {
                     return reg.get<ECS::Components::Transform2D>(e).transform;
                   });
    m_renderer.write2dTransformsToBuffer(m_2dTransforms);
  }
  m_renderer.render();
}

void RenderSystem::enqueueRenderable2D(entt::entity e) {
  std::cout << static_cast<uint32_t>(e) << std::endl;
  m_2dRenderables.push_back(e);
  m_2dRenderablesChanged = true;
}

void RenderSystem::dequeueRenderable2D(entt::entity e) {
  std::cout << static_cast<uint32_t>(e) << std::endl;
  auto it = std::find(m_2dRenderables.begin(), m_2dRenderables.end(), e);
  if (it != m_2dRenderables.end()) {
    *it = m_2dRenderables.back();
    m_2dRenderables.pop_back();
    m_2dRenderablesChanged = true;
  }
}
} // namespace Aether