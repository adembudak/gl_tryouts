#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 tPosition;

layout (location = 3) uniform mat4x4 view;
layout (location = 4) uniform mat4x4 projection;

out vec2 textureCoordinate;

void main() {
  textureCoordinate = tPosition;
  gl_Position = projection * view * vec4(vPosition, 1.0);
}

