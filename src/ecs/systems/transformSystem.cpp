#include "transformSystem.hpp"
#include "ecs/componets/transform.hpp"

#include <entt/entity/registry.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Aether::ECS::Systems {
void TransformSystem::update(entt::registry &reg) {
  const auto view =
      reg.view<Components::Position, Components::Rotation, Components::Scale>();
  view.each([&reg](entt::entity e, auto &p, auto &r, auto &s) {
    auto transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(p.x, p.y, 0));
    transform =
        glm::rotate(transform, glm::radians(r.angle), glm::vec3(0.f, 0.f, 1.f));
    transform = glm::scale(transform, glm::vec3(s.x, s.y, 1.f));
    reg.emplace_or_replace<Components::Transform2D>(e, transform);
  });
}

} // namespace Aether::ECS::Systems