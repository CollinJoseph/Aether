#pragma once
#include <entt/entity/registry.hpp>

namespace Aether::ECS::Systems {
class TransformSystem {
public:
  void update(entt::registry &reg);
};
} // namespace Aether::ECS::Systems