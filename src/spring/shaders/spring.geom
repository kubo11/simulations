#version 460 core
layout(lines) in;
layout(triangle_strip, max_vertices = 192) out;

#define PI 3.14159265359
#define COUNTSMALL 32

in vec3 outNorm[];
in vec3 outTangent[];

uniform float rsmall;

mat3 get_rotation_mat(float angle, vec3 axis) {
  mat3 W = mat3(0.0, -axis.z, axis.y, axis.z, 0.0, -axis.x, -axis.y, axis.x, 0.0);
  return mat3(1.0) + sin(angle) * W + 2.0 * pow(sin(angle / 2.0), 2.0) * W * W;
}

void main() {
  for (int i = 0; i < COUNTSMALL; ++i) {
    float angle1 = float(i) / COUNTSMALL * 2.0 * PI;
    float angle2 = float((i + 1) % int(COUNTSMALL)) / COUNTSMALL * 2.0 * PI;

    // triangle 1
    gl_Position = gl_in[0].gl_Position + vec4(rsmall * get_rotation_mat(angle1, outTangent[0]) * outNorm[0], 0.0);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + vec4(rsmall * get_rotation_mat(angle2, outTangent[1]) * outNorm[1], 0.0);
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4(rsmall * get_rotation_mat(angle2, outTangent[0]) * outNorm[0], 0.0);
    EmitVertex();
    EndPrimitive();
    // triangle 2
    gl_Position = gl_in[0].gl_Position + vec4(rsmall * get_rotation_mat(angle1, outTangent[0]) * outNorm[0], 0.0);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + vec4(rsmall * get_rotation_mat(angle1, outTangent[1]) * outNorm[1], 0.0);
    EmitVertex();
    gl_Position = gl_in[1].gl_Position + vec4(rsmall * get_rotation_mat(angle2, outTangent[1]) * outNorm[1], 0.0);
    EmitVertex();
    EndPrimitive();
  }
}