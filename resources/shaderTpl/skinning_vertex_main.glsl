  modelMatrix = mat4(0);
  for (int i = 0; i < 3; i++) {
    int jointIndex = int(aJointIndices[i]);
    float jointWeight = aJointWeights[i];
    modelMatrix += skinningMatrices.matrices[jointIndex] * jointWeight;
  }
