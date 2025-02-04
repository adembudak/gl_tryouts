#version 460 core
#extension all: warn

#pragma optimize(off)
#pragma debug(on)

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec4 color;

layout (location = 2) uniform mat4x4 transform;
layout (location = 3) uniform mat4x4 scale;
layout (location = 4) uniform mat4x4 rotate;

out vec4 fColor;

const bool reality = false;

void afunc(const in float readonlyVariable, in float inVar, out float outVar, inout float inOutVar) {
  bool post_reality = true;

  if (reality) {
    post_reality = true;
  }
  else {
  post_reality = false;
  }

  int i = 3;

  for(int k = 0; k < i; ++k) {
  }

  switch(i) {
    case 0: break;
    case 1: break;
    case 2: break;
    default: break;
  }
}

const vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 translucent_red = vec4(red.rgb, 0.5);
const vec4 reduced_red = 0.5 * vec4(red.rgb, 0.5);

bool b;
uint ui;
int  i;
float f = 1.0f;
double d = 1.0;

vec2 v2 = vec2(0.0, 0.0);
ivec2 iv2 = ivec2(v2);

vec4 rgba = vec4(0.0);
vec3 rgb = vec3(rgba); // truncated, discard 4th element?

float r = rgb.r = rgb[0];

vec3 color_ =rgb;
vec3 color_reverse = color_.bgr;

vec3 luminance = rgb.rrr;

vec3 white = vec3(1.0); // vec3(1.0, 1.0, 1.0);
vec4 translucent = vec4(white, 0.5);

mat3 m = mat3(4.0);  /* 4.0, 0.0, 0.0
                        0.0, 4.0, 0.0
                        0.0, 0.0, 4.0 */

const mat4 I = mat4(1.0);

bvec4 booleans = bvec4(true, true, true, true);
bool ret = any(booleans);
bvec4 allFalse = not(booleans);

vec3 col1 = vec3(1.0, 2.0, 3.0);
vec3 col2 = vec3(4.0, 5.0, 6.0);
vec3 col3 = vec3(7.0, 8.0, 9.0);

mat3 m2 = mat3(col1, col2, col3); /* 1.0 4.0 7.0
				     2.0 5.0 8.0
				     3.0 6.0 9.0 */

mat3 m3 = {
  {1.0, 2.0, 3.0},
  {4.0, 5.0, 6.0},
  {7.0, 8.0, 9.0}
};

struct Particle {
  float lifetime;
  vec3 position;
  vec3 velocity;
};

dvec2 dv2;
dvec3 dv3;
dvec4 dv4;

dvec2[2] dv2_v;
int size_of_array = dv2_v.length();

vec4[2] get4DPosition(in vec2 p) {
  vec4[2] t = {vec4(p, 0.0, 1.0), vec4(1.0)};
  return t;
}

vec2 unit(in vec2 p) { return vec2(0.0, 0.0); }
vec2 unit(in vec3 p) { return vec2(p.x, p.y); }

mat4x4 matrixx = mat4x4(1.0f); // matCxR

void main() {
  matrixx[0][0] = 0.5;
  matrixx[1][1] = 0.5;
  fColor = color;
  gl_Position = scale * transform * rotate * vec4(vPosition, 0.0, 1.0);
}

