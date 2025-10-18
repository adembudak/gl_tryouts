#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

layout (location = 1) in vec2 tPosition;
layout (location = 0) in vec3 vertexPosition;

layout (location = 2) uniform mat4x4 transform;
layout (location = 3) uniform mat4x4 view;
layout (location = 4) uniform mat4x4 projection;

out vec2 textureCoordinate;

void main() {
  textureCoordinate = tPosition;
  gl_Position = projection * view * transform * vec4(vPosition, 1.0);
  gl_Position = projection * view * transform * vec4(vertexPosition, 1.0);
}

