#version 460 core

in TES_OUT {
  vec3 pos;
  vec3 norm;
  vec2 tex;
}
fsIn;

const vec3 lightPos = vec3(2.0, 2.1, 0.0);
const vec3 ambientColor = vec3(0.3, 0.3, 0.3);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float kd = 0.7, ks = 0.7, m = 120.0;

uniform mat4 view;
uniform sampler2D diffuse;

out vec4 FragColor;

void main() {
  vec4 baseColor = texture(diffuse, fsIn.tex);
  vec3 N = normalize(fsIn.norm);
  vec3 camPos = vec3(inverse(view) * vec4(0.0, 0.0, 0.0, 1.0));
  vec3 V = normalize(camPos.xyz - fsIn.pos);
  vec3 finalColor = baseColor.rgb * ambientColor;
  vec3 L = normalize(lightPos.xyz - fsIn.pos);
  vec3 H = normalize(V + L);
  finalColor += lightColor * baseColor.rgb * kd * clamp(dot(N, L), 0.0, 1.0);
  float nh = clamp(dot(N, H), 0.0, 1.0);
  nh = pow(nh, m) * ks;
  finalColor += lightColor * nh;
  FragColor =  vec4(clamp(finalColor, 0.0, 1.0), baseColor.a);
  //FragColor =  vec4(0.5 + 0.5 * fsIn.norm, baseColor.a);
}