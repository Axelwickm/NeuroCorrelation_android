#version 300 es

in float opacity;

uniform mat4 uMVPMatrix;

out vec4 f_color;

void main(){
  if (0.0 < opacity)
    f_color = vec4(0.45, 0.3, 0.45, opacity);
  else
    f_color = vec4(0.05, 0.05, 0.3, -opacity);
}
