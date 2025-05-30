#version 460 core

layout (location = 0) in vec2 position;

out vec2 uv;

void main() {
  gl_Position = vec4(position, 0.0, 1.0);
  uv = 0.5 * position + 0.5;
}