#version 460 core
#extension all: warn

layout (binding = 0) uniform sampler2D s;

in vec2 textureCoordinate;
in vec3 fragmentPosition;

out vec4 color;

void main() {
  color = texture(s, textureCoordinate);
}
