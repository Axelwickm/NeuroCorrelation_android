#version 300 es

uniform mat4 uMVPMatrix;

in vec4 vPosition;
in float vPotential;

out float potential;

void main() {
  if (isnan(vPosition.x)){
    return;
  }
  gl_Position = uMVPMatrix * vPosition;
  potential = vPotential;
}