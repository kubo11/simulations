#version 460 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inCol;

uniform mat4 model, view, projection;

layout(location = 0) out vec3 fsCol;

void main() {
  gl_Position = projection * view * model * vec4(inPos, 1.0);
  fsCol = inCol;
}