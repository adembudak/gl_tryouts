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

in vec3 normal;
in vec2 textureCoordinate;

out vec4 fragmentColor;

void main() {
  if (baseColorTexture.hasValue)  {
     vec4 textureColor = texture(baseColorTexture.sampler, textureCoordinate);
     fragmentColor = textureColor * baseColor;
  } else {
     fragmentColor = baseColor;
  }
}
