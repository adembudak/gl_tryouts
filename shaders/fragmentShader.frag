#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

uniform vec4 baseColor;
uniform float metallic;
uniform float roughness;

layout(binding = 0)
uniform sampler2D baseColorTextureSampler;

in vec3 normal;
in vec2 textureCoordinate;

out vec4 fragmentColor;

void main() {
  vec4 textureColor = texture(baseColorTextureSampler, textureCoordinate);

  fragmentColor = textureColor * baseColor;
}
