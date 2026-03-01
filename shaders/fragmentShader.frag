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
  fragmentColor = vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
}
