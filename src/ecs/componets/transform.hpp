#pragma once
#include <glm/glm.hpp>

namespace Engine::ECS::Components {
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
} // namespace Engine::ECS::Components