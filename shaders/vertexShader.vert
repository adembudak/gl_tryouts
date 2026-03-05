#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 textureCoordinate_0;

layout (location = 3) uniform mat4x4 transform;
layout (location = 4) uniform mat4x4 view;
layout (location = 5) uniform mat4x4 projection;

out vec3 normal;
out vec2 textureCoordinate;

void main() {
  normal = vertexNormal;
  textureCoordinate = textureCoordinate_0;

  gl_Position = projection * view * transform * vec4(vertexPosition, 1.0);
}

