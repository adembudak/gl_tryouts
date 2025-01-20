#version 460 core
#extension all: warn

in vec4 fColor;
out vec4 color;

void main() {
  color = fColor;
}
