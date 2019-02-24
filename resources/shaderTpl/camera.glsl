struct Camera {
  vec3 position;
  uvec2 screenSize;
  mat4 viewMatrix;
  mat4 projectionMatrix;
};

layout (std140) uniform CameraBlock {
  Camera camera;
};
