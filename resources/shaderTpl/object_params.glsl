struct ObjectParams {
  mat4 model;
  mat4 normalMatrix;
  vec2 uvScale;
  vec2 uvOffset;
  uint layer;
};

layout (std140) uniform ObjectParamsBlock {
  ObjectParams objectParams;
};
