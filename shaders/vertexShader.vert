#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec4 color;

layout (location = 2) uniform mat4x4 transform;

out vec4 fColor;

void main() {
  fColor = color;
  gl_Position = transform * vec4(vPosition, 0.0, 1.0);
}

