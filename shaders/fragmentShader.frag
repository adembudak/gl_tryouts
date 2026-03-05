#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

layout (location = 6) uniform dvec4 baseColor;
layout (location = 7) uniform double metallic;
layout (location = 8) uniform double roughness;

layout (location = 9) uniform sampler2D baseColorTextureSampler;

in vec3 normal;
in vec2 textureCoordinate;

out vec4 fragmentColor;

void main() {
  vec4 baseColor = vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);

  vec4 textureColor = texture(baseColorTextureSampler, textureCoordinate);

  fragmentColor = textureColor * baseColor;
}
