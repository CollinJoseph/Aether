#pragma once
#include <glm/glm.hpp>

namespace Aether::ECS::Components {
struct Position {
  float x;
  float y;
};

struct Rotation {
  float angle;
};

struct Scale {
  float x;
  float y;
};

struct Transform2D {
  glm::mat4 transform;
};
} // namespace Aether::ECS::Components