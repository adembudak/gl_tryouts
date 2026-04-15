#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexTangent;

in vec2 TEXCOORD_0;

uniform mat4x4 transform;
uniform mat4x4 view;
uniform mat4x4 projection;

out vec3 normal;
out vec4 tangent;
out vec2 textureCoordinate;

void main() {
  normal = vertexNormal;
  tangent = vertexTangent;
  textureCoordinate = TEXCOORD_0;

  gl_Position = projection * view * transform * vec4(vertexPosition, 1.0);
}

