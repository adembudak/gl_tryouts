#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

uniform vec4 baseColor;
uniform float metallic;
uniform float roughness;

struct BaseColorTexture_t {
  bool hasValue;
  sampler2D sampler;
};
uniform BaseColorTexture_t baseColorTexture;

struct MetallicRoughnessTexture_t {
  bool hasValue;
  sampler2D sampler;
};
uniform MetallicRoughnessTexture_t metallicRoughnessTexture;

in vec3 normal;
in vec2 textureCoordinate;

out vec4 fragmentColor;

void main() {
  vec4 baseColorFinal = baseColor;
  if(baseColorTexture.hasValue) {
    baseColorFinal *= texture(baseColorTexture.sampler, textureCoordinate);
  }

  float roughnessFinal = roughness;
  float metallicFinal = metallic;
  if(metallicRoughnessTexture.hasValue) {
    vec4 sampled = texture(metallicRoughnessTexture.sampler, textureCoordinate);
    roughnessFinal = roughness * sampled.g;
    metallicFinal = metallic * sampled.b;
  }

  vec3 base = baseColorFinal.rgb;
  vec3 F0 = mix(vec3(0.04f), base, metallicFinal);
  vec3 diffuseColor = base * (1.0f - metallicFinal);

  fragmentColor = baseColorFinal;
}
