#ifndef SIMULATIONS_COMMON_VERTICES
#define SIMULATIONS_COMMON_VERTICES

#include "pch.hh"

#include "vertex_array.hh"

struct Vertex {
  glm::vec3 position;

  Vertex() : position{0.0f, 0.0f, 0.0f} {}
  Vertex(const glm::vec3& position) : position(position) {}

  static std::vector<VertexAttribute> get_vertex_attributes() { return {{.size = 3, .type = GL_FLOAT}}; }
};

struct NormalVertex {
  glm::vec3 position;
  glm::vec3 normal;

  NormalVertex() : position{0.0f, 0.0f, 0.0f}, normal{0.0f, 0.0f, 0.0f} {}
  NormalVertex(const glm::vec3& position, const glm::vec3& normal) : position(position), normal(normal) {}

  static std::vector<VertexAttribute> get_vertex_attributes() {
    return {{.size = 3, .type = GL_FLOAT}, {.size = 3, .type = GL_FLOAT}};
  }
};

struct ColorVertex {
  glm::vec3 position;
  glm::vec3 color;

  ColorVertex() : position{0.0f, 0.0f, 0.0f}, color{0.0f, 0.0f, 0.0f} {}
  ColorVertex(const glm::vec3& position, const glm::vec3& color) : position(position), color(color) {}

  static std::vector<VertexAttribute> get_vertex_attributes() {
    return {{.size = 3, .type = GL_FLOAT}, {.size = 3, .type = GL_FLOAT}};
  }
};

struct TextureVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex;

  TextureVertex() : position{0.0f, 0.0f, 0.0f}, normal{0.0f, 0.0f, 0.0f}, tex{0.0f, 0.0f} {}
  TextureVertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& tex)
      : position(position), normal(normal), tex(tex) {}

  static std::vector<VertexAttribute> get_vertex_attributes() {
    return {{.size = 3, .type = GL_FLOAT}, {.size = 3, .type = GL_FLOAT}, {.size = 2, .type = GL_FLOAT}};
  }

  bool operator==(const TextureVertex& other) const {
      return std::memcmp(this, &other, sizeof(TextureVertex)) == 0;
  }
};

namespace std {
  template <>
  struct hash<TextureVertex> {
      size_t operator()(const TextureVertex& vertex) const {
          size_t posHash = hash<float>()(vertex.position.x) ^
                            hash<float>()(vertex.position.y) ^
                            hash<float>()(vertex.position.z);
          size_t normalHash = hash<float>()(vertex.normal.x) ^
                              hash<float>()(vertex.normal.y) ^
                              hash<float>()(vertex.normal.z);
          size_t texHash = hash<float>()(vertex.tex.x) ^
                                hash<float>()(vertex.tex.y);
          return posHash ^ (normalHash << 1) ^ (texHash << 2);
      }
  };
}

#endif // SIMULATIONS_COMMON_VERTICES