#version 460 core
layout(lines) in;
layout(triangle_strip, max_vertices = 192) out;

#define PI 3.14159265359
#define COUNTSMALL 16

in VS_OUT {
  vec3 pos;
  vec3 norm;
  vec3 tangent;
}
gsIn[];

uniform mat4 view, projection;
uniform float rsmall;

out GS_OUT {
  vec3 pos;
  vec3 norm;
}
gsOut;

mat3 get_rotation_mat(float angle, vec3 axis) {
  mat3 W = mat3(0.0, -axis.z, axis.y, axis.z, 0.0, -axis.x, -axis.y, axis.x, 0.0);
  return mat3(1.0) + sin(angle) * W + 2.0 * pow(sin(angle / 2.0), 2.0) * W * W;
}

void create_vertex(float angle, vec3 pos, vec3 norm, vec3 tangent) {
  gsOut.norm = get_rotation_mat(angle, tangent) * norm;
  gsOut.pos = pos + rsmall * gsOut.norm;
  gl_Position = projection * view * vec4(gsOut.pos, 1.0);
  EmitVertex();
}

void main() {
  for (int i = 0; i < COUNTSMALL; ++i) {
    float angle1 = float(i) / COUNTSMALL * 2.0 * PI;
    float angle2 = float((i + 1) % int(COUNTSMALL)) / COUNTSMALL * 2.0 * PI;

    // triangle 1
    create_vertex(angle1, gsIn[0].pos, gsIn[0].norm, gsIn[0].tangent);
    create_vertex(angle2, gsIn[1].pos, gsIn[1].norm, gsIn[1].tangent);
    create_vertex(angle2, gsIn[0].pos, gsIn[0].norm, gsIn[0].tangent);
    EndPrimitive();
    // triangle 2
    create_vertex(angle1, gsIn[0].pos, gsIn[0].norm, gsIn[0].tangent);
    create_vertex(angle1, gsIn[1].pos, gsIn[1].norm, gsIn[1].tangent);
    create_vertex(angle2, gsIn[1].pos, gsIn[1].norm, gsIn[1].tangent);
    EndPrimitive();
  }
}