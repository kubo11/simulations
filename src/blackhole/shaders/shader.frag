#version 460 core

const float EPS = 1.0e-5;
const int MAX_ITERS = 1000;
const int INTEGRATION_STEPS = 1024;
const float PI = 3.14159265358979323846;

const vec3 blackHolePosition = vec3(0.0, 0.0, 0.0);

in vec2 uv;

uniform mat4 invProjView;
uniform vec3 cameraPosition;
uniform samplerCube skybox;
uniform float blackHoleMass;

out vec4 FragColor;

struct Ray {
  vec3 beg;
  vec3 dir;
};

Ray getRay() {
  Ray ray;

  vec2 xy = 2.0 * uv - 1.0;

  vec4 from = invProjView * vec4(xy, -1.0, 1.0);
  vec4 to = invProjView * vec4(xy, 1.0, 1.0);
  from /= from.w;
  to /= to.w;

  ray.beg = from.xyz;
  ray.dir = normalize(to.xyz - from.xyz);

  return ray;
}

float distance(vec3 point, Ray ray) {
  float t = dot(ray.dir, point - ray.beg) / dot(ray.dir, ray.dir);
  return length(point - (ray.beg + t * ray.dir));
}

float f(float w, float M, float b) {
  return 1.0 - w * w * (1.0 - 2.0 * w * M / b);
}

float df(float w, float M, float b) {
  return 2.0 * w * (3.0 * M * w / b - 1.0);
}

float fs(float w, float M, float b) {
  return pow(f(w, M, b), -0.5);
}

float FindUpperLimit(float M, float b) {
  float w = 1.0;
  float limit;
  int count = 0;
  
  while(true) {
    count++;
    limit = w - f(w, M, b) / df(w, M, b);
    
    if(abs(limit - w) < EPS) break;
    if(count > MAX_ITERS) return -1.0;
    
    w = limit;
  }
  
  return limit;
}

float integrate(float upper, float M, float b) {
  float dw = upper / INTEGRATION_STEPS;
  float sum = 0.0;
  for(int i = 1; i < INTEGRATION_STEPS; i++) {
    sum += fs(i * dw - dw / 2.0, M, b);
  }
  return sum * dw;
}

mat4 rotationMatrix(vec3 u, float theta) {
  u = normalize(u);
  float s = sin(theta);
  float c = cos(theta);
  float c1 = 1.0 - c;

  return mat4(u.x * u.x * c1 + c,        u.x * u.y * c1 - u.z * s,  u.x * u.z * c1 + u.y * s,  0.0,
              u.x * u.y * c1 + u.z * s,  u.y * u.y * c1 + c,        u.y * u.z * c1 - u.x * s,  0.0,
              u.x * u.z * c1 - u.y * s,  u.y * u.z * c1 + u.x * s,  u.z * u.z * c1 + c,        0.0,
              0.0,                       0.0,                       0.0,                       1.0);
}

void main() {
  Ray ray = getRay();
  float b = distance(blackHolePosition, ray);

  float upperLimit = FindUpperLimit(blackHoleMass, b);
  if(upperLimit < 0.0) {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }

  float theta = 2.0 * integrate(upperLimit, blackHoleMass, b) - PI;

  if(abs(theta) > PI || isnan(theta) || isinf(theta)) {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  } else {
    vec3 u = cross(normalize(blackHolePosition-cameraPosition), ray.dir);
    vec3 dir = normalize(vec3(rotationMatrix(u, theta) * vec4(ray.dir, 0.0)));
    FragColor = texture(skybox, dir);
  }
}