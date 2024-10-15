#ifndef SIMULATIONS_COMMON_BUFFER
#define SIMULATIONS_COMMON_BUFFER

#include "pch.hh"

template <class T = float>
class Buffer {
 public:
  enum class Type { Array = 0, ElementArray = 1, Uniform = 2, Texture = 3, ShaderStorage = 4 };
  enum class AccessMode { Read = 0, Write = 1, ReadWrite = 2 };
  enum class UsageHint { Draw = 0, Read = 1, Copy = 2 };
  enum class FrequencyHint { Static = 0, Dynamic = 1, Stream = 2 };

  Buffer(Type type = Type::Array, UsageHint usage = UsageHint::Draw, FrequencyHint frequency = FrequencyHint::Static)
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
  const Type get_type() const { return m_type; }

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
  void copy_subregion(unsigned int offset, unsigned int size, T* data) {
    glBufferSubData(buffer_type_to_gl(m_type), m_id, offset, size, data);
    glCheckError();
  }
  void copy_subregion(unsigned int offset, const std::vector<T>& data) {
    copy_subregion(offset, data.size() * sizeof(T), data.data());
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

  const Type m_type;
  const UsageHint m_usage;
  const FrequencyHint m_frequency;

  static GLenum buffer_type_to_gl(Type type) {
    switch (type) {
      case Type::Array:
        return GL_ARRAY_BUFFER;
      case Type::ElementArray:
        return GL_ELEMENT_ARRAY_BUFFER;
      case Type::Uniform:
        return GL_UNIFORM_BUFFER;
      case Type::Texture:
        return GL_TEXTURE_BUFFER;
      case Type::ShaderStorage:
        return GL_SHADER_STORAGE_BUFFER;

      default:
        return GL_NONE;
    }
  }

  static GLenum buffer_access_mode_to_gl(AccessMode mode) {
    switch (mode) {
      case AccessMode::Read:
        return GL_READ_ONLY;
      case AccessMode::Write:
        return GL_WRITE_ONLY;
      case AccessMode::ReadWrite:
        return GL_READ_WRITE;

      default:
        return GL_NONE;
    }
  }

  static GLenum buffer_hint_to_gl(FrequencyHint frequency, UsageHint usage) {
    switch (frequency) {
      case FrequencyHint::Static:
        switch (usage) {
          case UsageHint::Draw:
            return GL_STATIC_DRAW;
          case UsageHint::Read:
            return GL_STATIC_READ;
          case UsageHint::Copy:
            return GL_STATIC_COPY;

          default:
            return GL_NONE;
        }
      case FrequencyHint::Dynamic:
        switch (usage) {
          case UsageHint::Draw:
            return GL_DYNAMIC_DRAW;
          case UsageHint::Read:
            return GL_DYNAMIC_READ;
          case UsageHint::Copy:
            return GL_DYNAMIC_COPY;

          default:
            return GL_NONE;
        }
      case FrequencyHint::Stream:
        switch (usage) {
          case UsageHint::Draw:
            return GL_STREAM_DRAW;
          case UsageHint::Read:
            return GL_STREAM_READ;
          case UsageHint::Copy:
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