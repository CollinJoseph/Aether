#pragma once
#include <entt/entity/registry.hpp>

namespace Engine::ECS::Systems {
class TransformSystem {
public:
  void update(entt::registry &reg);
};
} // namespace Engine::ECS::Systems