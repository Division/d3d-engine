[vertex]
#version 300 es

in vec3 aPosition;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;

void main(void) {
  gl_PointSize = 5.0;
  gl_Position = uPMatrix * uMVMatrix * vec4(aPosition, 1.0);
}

[fragment]
#version 300 es

precision highp float;

out vec4 fragmentColor;

void main(void) {
  fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);
}