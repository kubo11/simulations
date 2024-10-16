#version 460 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inCol;

layout(location = 0) out vec3 outCol;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(inPos, 1.0);
  outCol = inCol;
}