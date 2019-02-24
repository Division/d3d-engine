[vertex]

attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aTexCoord0;

attribute vec3 aJointWeights;
attribute vec3 aJointIndexes;

uniform vec3 uLightDir;

varying vec3 vNormal_cameraspace;
varying vec3 vEyeDirection_cameraspace;
varying vec3 vLightDir_cameraspace;

varying vec2 vTexCoord0;

const int MAX_JOINTS = 60;
const int JOINTS_PER_VERTEX = 3;

uniform mat4 uViewMatrix;
uniform mat4 uPMatrix;
uniform mat4 uJointTransforms[MAX_JOINTS];

void main(void) {
  vec4 position = vec4(0.0, 0.0, 0.0, 1.0);
  vec4 normal = vec4(0.0, 0.0, 0.0, 0.0); // w component is zero so translation will be skipped

  for (int i = 0; i < JOINTS_PER_VERTEX; i++) {
    float jointWeight = aJointWeights[i];
    int jointIndex = int(aJointIndexes[i]);
    mat4 jointMatrix = uJointTransforms[jointIndex];
    position += jointMatrix * vec4(aPosition, 1.0) * jointWeight;
    normal += jointMatrix * vec4(aNormal, 0.0) * jointWeight;
  }

  normal = normalize(normal);
  vec4 position_cameraspace = uViewMatrix * vec4(position.xyz, 1.0);

  vNormal_cameraspace = (uViewMatrix * normal).xyz;
  vLightDir_cameraspace = normalize(vec3(uViewMatrix * vec4(uLightDir, 0.0)));
  vEyeDirection_cameraspace = vec3(0, 0, 0) - position_cameraspace.xyz; // vector to the camera

  vTexCoord0 = aTexCoord0;
  gl_Position = uPMatrix * position_cameraspace;
}

[fragment]

precision mediump float;

uniform sampler2D uTexture0;

varying vec3 vNormal_cameraspace;
varying vec3 vEyeDirection_cameraspace;
varying vec3 vLightDir_cameraspace;
varying vec2 vTexCoord0;

void main(void) {
  vec3 uLightColor = vec3(1.0, 1.0, 1.0);
  vec4 ambient = vec4(0.0, 0.0, 0.0, 0.0);

  vec4 texture0 = texture2D(uTexture0, vec2(vTexCoord0.s, vTexCoord0.t));

  vec3 normal_cameraspace = normalize(vNormal_cameraspace);

  vec3 E = normalize(vEyeDirection_cameraspace);
  vec3 reflect = reflect(vLightDir_cameraspace, normal_cameraspace);
  float cosAlpha = clamp(dot(E, reflect), 0.0, 1.0);
  vec3 specular = pow(cosAlpha, 8.0) * vec3(1.0, 1.0, 1.0);

  float lightValue = clamp(dot(-vLightDir_cameraspace, normal_cameraspace), 0.0, 1.0);

  gl_FragColor = (ambient + vec4(uLightColor * lightValue, 1.0)) * texture0;// + vec4(specular, 1.0);
}