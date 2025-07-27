#include "resourceLoader.hpp"
#include "pch/pch.hpp"

#include <unordered_map>

namespace Engine::Resources {
class ResourceLoader {
public:
  void loadTexture(const std::string &id, const std::string &path) {}

private:
  void registerResource(const std::string &id, const std::string &path) {}

private:
  std::unordered_map<std::string, std::string> resources;
};
} // namespace Engine::Resources