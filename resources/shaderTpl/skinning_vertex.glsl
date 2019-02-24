in vec3 aJointWeights;
in vec3 aJointIndices;

struct SkinningMatrices {
  mat4 matrices[70];
};

layout (std140) uniform SkinningMatricesBlock {
  SkinningMatrices skinningMatrices;
};
