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
  float strength;
};
uniform OcclusionTexture_t occlusionTexture;

struct EmissiveTexture_t {
  bool isDefined;
  sampler2D sampler;
};
uniform EmissiveTexture_t emissiveTexture;

in vec3 normal;
in vec4 tangent;
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

  vec3 N = normalize(normal);
  if(normalTexture.isDefined) {
    vec3 n = texture(normalTexture.sampler, textureCoordinate).rgb;
    n = n * 2.0f - 1.0f;
    n *= normalTexture.scale;
    n = normalize(n);
  }

  if(occlusionTexture.isDefined) {}
  if(emissiveTexture.isDefined) {}

  vec3 base = baseColorFinal.rgb;
  vec3 F0 = mix(vec3(0.04f), base, metallicFinal);
  vec3 diffuseColor = base * (1.0f - metallicFinal);

  fragmentColor = baseColorFinal;
}
