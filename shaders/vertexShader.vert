#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

layout (location = 0) in vec3 vPosition;

layout (location = 1) uniform mat4x4 transform;
layout (location = 2) uniform mat4x4 view;
layout (location = 3) uniform mat4x4 projection;

void main() {
  gl_Position = projection * view * transform * vec4(vPosition, 1.0);
}

