#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

struct PBRMetallicRoughness_t {
  vec4 baseColor;
  float metallic;
  float roughness;
};
uniform PBRMetallicRoughness_t pbr;

struct BaseColorTexture_t {
  bool isDefined;
  sampler2D sampler;
};
uniform BaseColorTexture_t baseColorTexture;

struct MetallicRoughnessTexture_t {
  bool isDefined;
  sampler2D sampler;
};
uniform MetallicRoughnessTexture_t metallicRoughnessTexture;

struct NormalTexture_t {
  bool isDefined;
  sampler2D sampler;
  float scale;
};
uniform NormalTexture_t normalTexture;

struct OcclusionTexture_t {
  bool isDefined;
  sampler2D sampler;
};
uniform OcclusionTexture_t occlusionTexture;

in vec3 normal;
in vec2 textureCoordinate;

out vec4 fragmentColor;

void main() {
  vec4 baseColorFinal = pbr.baseColor;
  if(baseColorTexture.isDefined) {
    baseColorFinal *= texture(baseColorTexture.sampler, textureCoordinate);
  }

  float roughnessFinal = pbr.roughness;
  float metallicFinal = pbr.metallic;
  if(metallicRoughnessTexture.isDefined) {
    vec4 sampled = texture(metallicRoughnessTexture.sampler, textureCoordinate);
    roughnessFinal = pbr.roughness * sampled.g;
    metallicFinal = pbr.metallic * sampled.b;
  }

  if(normalTexture.isDefined) {
    vec4 sampled = texture(normalTexture.sampler, textureCoordinate);
  }

  vec3 base = baseColorFinal.rgb;
  vec3 F0 = mix(vec3(0.04f), base, metallicFinal);
  vec3 diffuseColor = base * (1.0f - metallicFinal);

  fragmentColor = baseColorFinal;
}
