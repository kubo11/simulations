#version 460 core

uniform mat4 model;
uniform float h0, l, r, rsmall, count;

out VS_OUT {
  vec3 pos;
  vec3 norm;
  vec3 tangent;
}
vsOut;

float dangle_ds(float l, float r, float h) { return sqrt(l * l - h * h) / r; }

vec3 get_position(float s, float r, float h, float a) { return vec3(r * cos(s * a), s * h, r * sin(s * a)); }

vec3 get_tangent(float s, float r, float h, float a) { return vec3(-a * r * sin(s * a), h, a * r * cos(s * a)); }

vec3 get_normal(float s, float r, float h, float a) {
  return vec3(-a * a * r * cos(s * a), 0.0, -a * a * r * sin(s * a));
}

void main() {
  float t = float(gl_VertexID) / count;
  float h = h0;
  float a = dangle_ds(l, r, h);
  vec3 curve_position = get_position(t, r, h, a);
  vec3 curve_normal = normalize(get_normal(t, r, h, a));
  vec3 curve_tangent = normalize(get_tangent(t, r, h, a));
  vsOut.pos = vec3(model * vec4(curve_position, 1.0));
  vsOut.norm = normalize(vec3(model * vec4(curve_normal, 0.0)));
  vsOut.tangent = normalize(vec3(model * vec4(curve_tangent, 0.0)));
}