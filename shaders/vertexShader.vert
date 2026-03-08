#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 textureCoordinate_0;

uniform mat4x4 transform;
uniform mat4x4 view;
uniform mat4x4 projection;

out vec3 normal;
out vec2 textureCoordinate;

void main() {
  normal = vertexNormal;
  textureCoordinate = textureCoordinate_0;

  gl_Position = projection * view * transform * vec4(vertexPosition, 1.0);
}

