#version 460 core
#extension all: warn

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec4 color;

out vec4 fColor;

void main() {
  fColor = color;
  gl_Position = vec4(vPosition, 0.0, 1.0);
}

