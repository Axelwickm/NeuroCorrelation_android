#version 300 es

in float potential;
out vec4 color;
void main() {
  if (0.0 < potential)
    color = vec4(0.45, 0.3, 0.45, potential);
  else
    color = vec4(0.05, 0.05, 0.3, -potential);
}