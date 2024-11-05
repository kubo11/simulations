#version 460 core

in VS_OUT {
  vec3 pos;
  vec3 norm;
}
fsIn;

const vec3 lightPos = vec3(2.0, 2.0, 2.0);
const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightIntensity = 5.0;
const vec3 ambientColor = vec3(0.1, 0.1, 0.1);
const vec3 specularColor = vec3(1.0, 1.0, 1.0);
const float shininess = 8.0;

uniform vec4 color;
uniform mat4 view;

out vec4 FragColor;

void main() {
  vec3 N = normalize(fsIn.norm);
  vec3 L = lightPos - fsIn.pos;
  float distance = dot(L, L);
  L = normalize(L);

  float diffuse = max(dot(L, N), 0.0);
  float specular = 0.0;

  if (diffuse > 0.0) {
    vec3 V = vec3(normalize(inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)));

    vec3 H = normalize(L + V);
    specular = pow(max(dot(H, N), 0.0), shininess);
  }
  vec3 outColor = ambientColor + color.rgb * diffuse * lightColor * lightIntensity / distance +
                  specularColor * specular * lightColor * lightIntensity / distance;
  FragColor = vec4(outColor, color.a);
}