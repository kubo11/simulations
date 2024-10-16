#ifndef SIMULATIONS_COMMON_VERTEX_ARRAY
#define SIMULATIONS_COMMON_VERTEX_ARRAY

#include "pch.hh"

#include "buffer.hh"

struct VertexAttribute {
  GLuint size;
  GLenum type;
};

template <class T>
class VertexArray {
 public:
  VertexArray() : m_last_attribute(0), m_vertex_buffer(nullptr), m_element_buffer(nullptr) {
    glGenVertexArrays(1, &m_id);
    glCheckError();
  }

  VertexArray(std::unique_ptr<Buffer<T>> vertex_buffer, const std::vector<VertexAttribute>& vertex_attributes,
              std::unique_ptr<Buffer<unsigned int>> element_buffer = nullptr)
      : m_last_attribute(0), m_vertex_buffer(std::move(vertex_buffer)), m_element_buffer(std::move(element_buffer)) {
    glGenVertexArrays(1, &m_id);
    glCheckError();
    bind();
    attach_buffer(*m_vertex_buffer, vertex_attributes);
    unbind();
  }

  virtual ~VertexArray() { destroy(); }

  void destroy() {
    if (m_id) {
      glDeleteVertexArrays(1, &m_id);
      glCheckError();
    }
  }

  void bind() {
    glBindVertexArray(m_id);
    glCheckError();
    if (has_element_buffer()) {
      m_element_buffer->bind();
    }
  }

  void unbind() {
    glBindVertexArray(0);
    glCheckError();
    if (has_element_buffer()) {
      m_element_buffer->unbind();
    }
  }

  bool has_vertex_buffer() const { return m_vertex_buffer != nullptr; }

  Buffer<T>& get_vertex_buffer() {
    if (!has_vertex_buffer()) throw std::runtime_error("Invalid access: no vertex buffer");
    return *m_vertex_buffer;
  }

  bool has_element_buffer() const { return m_element_buffer != nullptr; }

  Buffer<unsigned int>& get_element_buffer() {
    if (!has_element_buffer()) throw std::runtime_error("Invalid access: no element buffer");
    return *m_element_buffer;
  }

  unsigned int get_draw_size() const {
    return has_element_buffer() ? m_element_buffer->get_count() : m_vertex_buffer->get_count();
  }

 protected:
  GLuint m_id;
  GLuint m_last_attribute;
  std::unique_ptr<Buffer<T>> m_vertex_buffer;
  std::unique_ptr<Buffer<unsigned int>> m_element_buffer;

  void attach_buffer(Buffer<T>& buffer, const std::vector<VertexAttribute>& attributes) {
    buffer.bind();
    GLuint stride = 0;
    for (GLuint i = 0; i < attributes.size(); ++i) {
      glEnableVertexAttribArray(m_last_attribute + i);
      glVertexAttribPointer(m_last_attribute + i, attributes[i].size, attributes[i].type, GL_FALSE, sizeof(T),
                            reinterpret_cast<void*>(stride));
      glCheckError();
      stride += attributes[i].size * glSizeofType(attributes[i].type);
    }
    buffer.unbind();
    m_last_attribute += attributes.size();
  }
};

#endif  // SIMULATIONS_COMMON_VERTEX_ARRAY