#include "model.hh"

Model::Model(std::unique_ptr<VertexArray<TextureVertex>> vertex_array, std::unique_ptr<Material> material) : m_vertex_array(std::move(vertex_array)), m_material(std::move(material)) {}

std::unique_ptr<Model> Model::load(const fs::path& path) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), path.parent_path().c_str());

  if (!warn.empty()) {
    std::cerr << warn << std::endl;
  }
  if (!err.empty()) {
      std::cerr << err << std::endl;
  }
  if (!success) {
      exit(1);
  }

  auto vertices = std::vector<TextureVertex>{};
  auto indices = std::vector<unsigned int>{};
  std::unordered_map<TextureVertex, unsigned int> unique_vertices;

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      TextureVertex vertex{};
      if (index.vertex_index >= 0) {
        auto idx = 3 * index.vertex_index;
        vertex.position = {attrib.vertices[idx], attrib.vertices[idx + 1], attrib.vertices[idx + 2]};
      }

      if (index.normal_index >= 0) {
        auto idx = 3 * index.normal_index;
        vertex.normal = {attrib.normals[idx], attrib.normals[idx + 1], attrib.normals[idx + 2]};
      } else {
        vertex.normal = {0.0f, 0.0f, 1.0f};
      }

      if (index.texcoord_index >= 0) {
        auto idx = 2 * index.texcoord_index;
        vertex.tex = {attrib.texcoords[idx], attrib.texcoords[idx + 1]};
      } else {
        vertex.tex = {0.0f, 0.0f};
      }

      if (!unique_vertices.contains(vertex)) {
        unique_vertices[vertex] = vertices.size();
        vertices.push_back(vertex);
      }

      indices.push_back(unique_vertices.at(vertex));
    }
  }

  std::unique_ptr<Material> main_material = nullptr;
  if (materials.size() > 0) {
    auto material = materials.front();
    std::unique_ptr<Texture> normal = nullptr;

    if (!material.normal_texname.empty()) {
      normal = Texture::load(path.parent_path() / fs::path(material.normal_texname), TextureConfig{});
    }

    if (!material.diffuse_texname.empty()) {
      auto diffuse = Texture::load(path.parent_path() / fs::path(material.diffuse_texname), TextureConfig{});
      main_material = std::make_unique<Material>(std::move(diffuse), std::move(normal));
    }
    else {
      main_material = std::make_unique<Material>(Colors::White, std::move(normal));
    }
  }
  else {
    main_material = std::make_unique<Material>();
  }

  return std::unique_ptr<Model>(new Model(std::move(VertexArray<TextureVertex>::create(vertices, indices, TextureVertex::get_vertex_attributes())), std::move(main_material)));
}

void Model::draw(ShaderProgram& shader) {
  m_vertex_array->bind();
  m_material->bind(shader);
  shader.set_and_commit_uniform_value("model", m_model_mat);
  glDrawElements(GL_TRIANGLES, m_vertex_array->get_draw_size(), GL_UNSIGNED_INT, 0);
  glCheckError();
  m_material->unbind();
  m_vertex_array->unbind();
}

const glm::mat4& Model::get_model_mat() const {
  return m_model_mat;
}

Material& Model::get_material() {
  return *m_material;
}

void Model::set_model_mat(const glm::mat4& model_mat) {
  m_model_mat = model_mat;
}

