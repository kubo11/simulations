#ifndef SIMULATIONS_COMMON_BUFFER
#define SIMULATIONS_COMMON_BUFFER

#include "pch.hh"

enum class BufferType { Array = 0, ElementArray = 1, Uniform = 2, Texture = 3, ShaderStorage = 4 };
enum class BufferAccessMode { Read = 0, Write = 1, ReadWrite = 2 };
enum class BufferUsageHint { Draw = 0, Read = 1, Copy = 2 };
enum class BufferFrequencyHint { Static = 0, Dynamic = 1, Stream = 2 };

template <class T = float>
class Buffer {
 public:
  Buffer(BufferType type = BufferType::Array, BufferUsageHint usage = BufferUsageHint::Draw,
         BufferFrequencyHint frequency = BufferFrequencyHint::Static)
      : m_data{}, m_size(0), m_type(type), m_usage(usage), m_frequency(frequency) {
    glGenBuffers(1, &m_id);
    glCheckError();
  }
  virtual ~Buffer() { destroy(); }

  void destroy() {
    if (m_id) {
      glDeleteBuffers(1, &m_id);
      glCheckError();
    }
  }

  const unsigned int get_size() const { return m_size; }
  const unsigned int get_count() const { return m_size / sizeof(T); }
  const BufferType get_type() const { return m_type; }

  void bind() {
    glBindBuffer(buffer_type_to_gl(m_type), m_id);
    glCheckError();
  };
  void unbind() {
    glBindBuffer(buffer_type_to_gl(m_type), 0);
    glCheckError();
  }

  void copy(unsigned int size, const T* data) {
    glBufferData(buffer_type_to_gl(m_type), size, data, buffer_hint_to_gl(m_frequency, m_usage));
    glCheckError();
    m_size = size;
  }
  void copy(const std::vector<T>& data) {
    copy(data.size() * sizeof(T), data.data());
    glCheckError();
  }
  void copy_subregion(unsigned int offset, unsigned int size, const T* data) {
    glBufferSubData(buffer_type_to_gl(m_type), offset, size, data);
    glCheckError();
  }
  void copy_subregion(unsigned int offset, const std::vector<T>& data) {
    copy_subregion(offset * sizeof(T), data.size() * sizeof(T), data.data());
    glCheckError();
  }

  void stage(std::vector<T>&& data) { m_data = data; }
  void flush() {
    copy(m_data);
    std::vector<T> empty;
    m_data.swap(empty);
  }
  bool is_staged() { return !m_data.empty(); }

 private:
  unsigned int m_size;
  std::vector<T> m_data;
  GLuint m_id;

  const BufferType m_type;
  const BufferUsageHint m_usage;
  const BufferFrequencyHint m_frequency;

  static GLenum buffer_type_to_gl(BufferType type) {
    switch (type) {
      case BufferType::Array:
        return GL_ARRAY_BUFFER;
      case BufferType::ElementArray:
        return GL_ELEMENT_ARRAY_BUFFER;
      case BufferType::Uniform:
        return GL_UNIFORM_BUFFER;
      case BufferType::Texture:
        return GL_TEXTURE_BUFFER;
      case BufferType::ShaderStorage:
        return GL_SHADER_STORAGE_BUFFER;

      default:
        return GL_NONE;
    }
  }

  static GLenum buffer_access_mode_to_gl(BufferAccessMode mode) {
    switch (mode) {
      case BufferAccessMode::Read:
        return GL_READ_ONLY;
      case BufferAccessMode::Write:
        return GL_WRITE_ONLY;
      case BufferAccessMode::ReadWrite:
        return GL_READ_WRITE;

      default:
        return GL_NONE;
    }
  }

  static GLenum buffer_hint_to_gl(BufferFrequencyHint frequency, BufferUsageHint usage) {
    switch (frequency) {
      case BufferFrequencyHint::Static:
        switch (usage) {
          case BufferUsageHint::Draw:
            return GL_STATIC_DRAW;
          case BufferUsageHint::Read:
            return GL_STATIC_READ;
          case BufferUsageHint::Copy:
            return GL_STATIC_COPY;

          default:
            return GL_NONE;
        }
      case BufferFrequencyHint::Dynamic:
        switch (usage) {
          case BufferUsageHint::Draw:
            return GL_DYNAMIC_DRAW;
          case BufferUsageHint::Read:
            return GL_DYNAMIC_READ;
          case BufferUsageHint::Copy:
            return GL_DYNAMIC_COPY;

          default:
            return GL_NONE;
        }
      case BufferFrequencyHint::Stream:
        switch (usage) {
          case BufferUsageHint::Draw:
            return GL_STREAM_DRAW;
          case BufferUsageHint::Read:
            return GL_STREAM_READ;
          case BufferUsageHint::Copy:
            return GL_STREAM_COPY;

          default:
            return GL_NONE;
        }

      default:
        return GL_NONE;
    }
  }
};

#endif  // SIMULATIONS_COMMON_BUFFER