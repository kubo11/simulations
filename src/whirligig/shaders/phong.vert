#version 460 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;

uniform mat4 model, view, projection;

out VS_OUT {
  vec3 pos;
  vec3 norm;
}
vsOut;

void main() {
  vsOut.pos = vec3(model * vec4(inPos, 1.0));
  vsOut.norm = vec3(model * vec4(inNorm, 0.0));
  gl_Position = projection * view * vec4(vsOut.pos, 1.0);
}