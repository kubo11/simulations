#version 460 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inTex;

uniform mat4 model, view, projection;
uniform vec3 points[64];

float cubeMin = -0.5, cubeMax = 0.5;

out VS_OUT {
  vec3 pos;
  vec3 norm;
  vec2 tex;
}
vsOut;

vec4 BerensteinBasis(float t) {
  float invT = 1.0 - t;
  return vec4(invT * invT * invT,
              3.0 * t * invT * invT,
              3.0 * t * t * invT,
              t * t * t);
}

vec4 dBerensteinBasis(float t) {
  float invT = 1.0 - t;
  return vec4(-3.0 * invT * invT,
              3.0 * invT * invT - 6.0 * t * invT,
              6.0 * t * invT - 3.0 * t * t,
              3.0 * t * t);
}

vec3 EvaluateBezier(vec4 basisU, vec4 basisV, vec4 basisW) {
  vec3 value = vec3(0.0, 0.0, 0.0);
  for (int i = 0; i < 4; ++i) {
    vec3 tmp1 = vec3(0.0, 0.0, 0.0);
    for (int j = 0; j < 4; ++j) {
      vec3 tmp2 = vec3(0.0, 0.0, 0.0);
      for (int k = 0; k < 4; ++k) {
        tmp2 += basisW[k] * points[i*16+j*4+k];
      }
      tmp1 += basisV[j] * tmp2;
    }
    value += basisU[i] * tmp1;
  }

  return value;
}

void main() {
  vec3 uvw = (inPos - cubeMin) / (cubeMax - cubeMin);
  vec4 basisU = BerensteinBasis(uvw.x);
  vec4 basisV = BerensteinBasis(uvw.y);
  vec4 basisW = BerensteinBasis(uvw.z);
  vec3 worldPos = EvaluateBezier(basisU, basisV, basisW);

  vec3 uvw2 = (0.99999 * inPos - cubeMin) / (cubeMax - cubeMin);
  vec4 basisU2 = BerensteinBasis(uvw2.x);
  vec4 basisV2 = BerensteinBasis(uvw2.y);
  vec4 basisW2 = BerensteinBasis(uvw2.z);
  vec3 worldPos2 = EvaluateBezier(basisU2, basisV2, basisW2);

  vec3 norm = normalize(worldPos - worldPos2);

  vsOut.pos = worldPos;
  vsOut.norm = norm;
  vsOut.tex = inTex;

  gl_Position = projection * view * vec4(worldPos, 1.0);
}