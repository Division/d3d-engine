[vertex]

attribute vec3 aPosition;
attribute vec4 aColor;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;

varying vec4 color;

void main(void) {
  color = vec4(aColor.rgb, 1.0);
  gl_Position = uPMatrix * uMVMatrix * vec4(aPosition, 1.0);
  gl_PointSize = aColor.a;
}

[fragment]

precision mediump float;

varying vec4 color;

void main(void) {
  gl_FragColor = color;
}