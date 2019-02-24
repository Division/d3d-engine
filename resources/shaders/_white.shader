[vertex]

#version 150

in vec3 aPosition;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;

void main(void) {
  gl_PointSize = 5.0;
  gl_Position = uPMatrix * uMVMatrix * vec4(aPosition, 1.0);
}

[fragment]

#version 150

out vec4 fragmentColor;

// precision mediump float;

void main(void) {
  fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);
}