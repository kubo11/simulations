#version 460 core

layout(location = 0) in vec3 fsCol;

out vec4 FragColor;

void main() { FragColor = vec4(fsCol, 1.0); }