#version 460 core

layout(quads, fractional_even_spacing) in;

out vec3 fsPos;
out vec3 fsNorm;
out vec2 fsTex;

out TES_OUT {
  vec3 pos;
  vec3 norm;
  vec2 tex;
}
tesOut;

uniform mat4 view, projection;

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

vec3 EvaluateBezier(vec4 basisU, vec4 basisV) {
  vec3 value = vec3(0.0, 0.0, 0.0);
  value = basisV.x * (gl_in[0].gl_Position.xyz * basisU.x + gl_in[1].gl_Position.xyz * basisU.y + gl_in[2].gl_Position.xyz * basisU.z + gl_in[3].gl_Position.xyz * basisU.w);
  value += basisV.y * (gl_in[4].gl_Position.xyz * basisU.x + gl_in[5].gl_Position.xyz * basisU.y + gl_in[6].gl_Position.xyz * basisU.z + gl_in[7].gl_Position.xyz * basisU.w);
  value += basisV.z * (gl_in[8].gl_Position.xyz * basisU.x + gl_in[9].gl_Position.xyz * basisU.y + gl_in[10].gl_Position.xyz * basisU.z + gl_in[11].gl_Position.xyz * basisU.w);
  value += basisV.w * (gl_in[12].gl_Position.xyz * basisU.x + gl_in[13].gl_Position.xyz * basisU.y + gl_in[14].gl_Position.xyz * basisU.z + gl_in[15].gl_Position.xyz * basisU.w);
  return value;
}

vec3 EvaluateBezier2(vec4 basisU, vec4 basisV) {
  vec3 value = vec3(0.0, 0.0, 0.0);
  for (int i = 0; i < 4; ++i) {
    vec3 tmp = vec3(0.0, 0.0, 0.0);
    for (int j = 0; j < 4; ++j) {
      tmp += basisU[j] * gl_in[i*4+j].gl_Position.xyz;
    }
    value += basisV[i] * tmp;
  }
  return value;
}

void main() {
  vec4 basisU = BerensteinBasis(gl_TessCoord.x);
  vec4 basisV = BerensteinBasis(gl_TessCoord.y);
  vec4 dBasisU = dBerensteinBasis(gl_TessCoord.x);
  vec4 dBasisV = dBerensteinBasis(gl_TessCoord.y);
  vec3 worldPos = EvaluateBezier2(basisU, basisV);
  vec3 tangent = normalize(EvaluateBezier2(dBasisU, basisV));
  vec3 biTangent = normalize(EvaluateBezier2(basisU, dBasisV));
  vec3 norm = normalize(cross(tangent, biTangent));

  gl_Position = projection * view * vec4(worldPos, 1.0);
  tesOut.pos = worldPos;
  tesOut.norm = norm;
  tesOut.tex = vec2(gl_TessCoord.x, gl_TessCoord.y);
}
