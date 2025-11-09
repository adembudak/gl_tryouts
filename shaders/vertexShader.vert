#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

layout (location = 3) uniform mat4x4 transform;
layout (location = 4) uniform mat4x4 view;
layout (location = 5) uniform mat4x4 projection;

out vec3 normal;
out vec3 fragmentPosition;

void main() {
  fragmentPosition = vec3(model * vec4(vertexPosition, 1.0);
  normal = mat3(transpose(inverse(model))) * vertexNormal;
  gl_Position = projection * view * transform * vec4(vertexPosition, 1.0);
}

