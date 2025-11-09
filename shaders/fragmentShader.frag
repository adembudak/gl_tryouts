#version 460 core
#extension all: warn

layout (location = 6) uniform vec4 baseColorFactor;
layout (location = 7) uniform double metallicFactor;
layout (location = 8) uniform double roughnessFactor;

in vec3 fragmentPosition;
in vec3 normal;

out vec4 fragmentColor;

void main() {
  fragmentColor = vec4(0.5, 0.5, 0.5, 1.0);
}
