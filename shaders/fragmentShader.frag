#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

layout (location = 6) uniform dvec4 baseColor;
layout (location = 7) uniform double metallic;
layout (location = 8) uniform double roughness;

in vec3 normal;
out vec4 fragmentColor;

void main() {
  fragmentColor = vec4(normal.x + 0.5, normal.y + 0.2, normal.z / 2.0, 1.0);
 // fragmentColor = vec4(0.5, 0.5, 0.5, 1.0);
}
