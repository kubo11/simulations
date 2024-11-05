#version 460 core
layout(location = 0) in vec3 inPos;

uniform mat4 model, view, projection;

void main() { gl_Position = projection * view * model * vec4(inPos, 1.0); }