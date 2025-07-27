#include "renderSystem.hpp"
#include "ecs/componets/transform.hpp"
#include <glm/gtx/matrix_decompose.hpp>

namespace Aether {

void RenderSystem::update(entt::registry &reg) {
  const auto view = reg.view<Engine::ECS::Components::Transform2D>();
  view.each([&](auto &transform) {
    m_quadTransforms.push_back(transform.transform);
  });
}

void RenderSystem::render(entt::registry &reg) {
  for (auto matrix : m_quadTransforms) {
    // Decompose
    glm::vec3 translation = glm::vec3(matrix[3]);
    glm::vec3 scale;
    scale.x = glm::length(glm::vec3(matrix[0]));
    scale.y = glm::length(glm::vec3(matrix[1]));
    scale.z = glm::length(glm::vec3(matrix[2]));

    glm::mat3 rotationMatrix;
    rotationMatrix[0] = glm::vec3(matrix[0]) / scale.x;
    rotationMatrix[1] = glm::vec3(matrix[1]) / scale.y;
    rotationMatrix[2] = glm::vec3(matrix[2]) / scale.z;
    glm::quat rotation = glm::quat_cast(rotationMatrix);

    std::cout << "Translation: " << translation.x << ", " << translation.y
              << ", " << translation.z << std::endl;
    std::cout << "Scale: " << scale.x << ", " << scale.y << ", " << scale.z
              << std::endl;

    std::cout << "Rotation (quaternion): " << rotation.x << ", " << rotation.y
              << ", " << rotation.z << ", " << rotation.w << std::endl;
    std::vector<glm::vec3> unitPlane = {
        {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}, {0.5f, 0.5f, 0.0f},
        {0.5f, 0.5f, 0.0f},   {-0.5f, 0.5f, 0.0f}, {-0.5f, -0.5f, 0.0f}};

    std::vector<glm::vec3> finalVertices;
    for (const auto &v : unitPlane) {
      glm::vec3 transformed = rotationMatrix * (v * scale) + translation;
      finalVertices.push_back(transformed);
      std::cout << "  (" << transformed.x << ", " << transformed.y << ", "
                << transformed.z << ")\n";
    }
  }
  m_renderer.drawFrame();
}
} // namespace Aether