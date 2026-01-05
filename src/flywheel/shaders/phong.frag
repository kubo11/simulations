#version 460 core

in VS_OUT {
  vec3 pos;
  vec3 norm;
}
fsIn;

const vec3 lightPos = vec3(10.0, 1.0, -0.5);
const vec3 ambientColor = vec3(0.7, 0.7, 0.7);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float kd = 0.5, ks = 0.7, m = 150.0;

uniform vec4 color;
uniform mat4 view;

out vec4 FragColor;

void main() {
  vec3 camPos = vec3(inverse(view) * vec4(0.0, 0.0, 0.0, 1.0));

  vec3 V = normalize(camPos.xyz - fsIn.pos);
  vec3 N = normalize(fsIn.norm);
  vec3 finalColor = color.rgb * ambientColor;
  vec3 L = normalize(lightPos.xyz - fsIn.pos);
  vec3 H = normalize(V + L);
  finalColor += lightColor * color.rgb * kd * clamp(dot(N, L), 0.0, 1.0);
  if (dot(N, vec3(0.0, 1.0, 0.0)) > 0.0) {
    float nh = clamp(dot(N, H), 0.0, 1.0);
    nh = pow(nh, m) * ks;
    finalColor += lightColor * nh;
  }
  FragColor =  vec4(clamp(finalColor, 0.0, 1.0), color.a);
}