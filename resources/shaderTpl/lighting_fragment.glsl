{% if USE_BUFFER_TEXTURE %}
uniform usamplerBuffer uLightGrid;
uniform usamplerBuffer uLightIndices;
{% else %}
uniform highp usampler2D uLightGrid;
uniform highp usampler2D uLightIndices;
{% endif %}

{% if SPECULAR_MAP %}uniform highp sampler2D uSpecularMap;{% endif %}

uniform highp sampler2D uProjectorTexture;
uniform highp sampler2D uShadowMap;

struct Light {
  vec3 position;
  float attenuation;
  vec3 color;
  float radius;
  vec3 direction;
  float coneAngle;
  mat4 projectionMatrix;
  vec2 shadowmapScale;
  vec2 shadowmapOffset;
  uint mask;
};

layout (std140) uniform LightBlock {
  Light lights[100];
};

struct Projector {
  vec3 position;
  float attenuation;
  vec4 color;
  vec2 scale;
  vec2 offset;
  vec2 shadowmapScale;
  vec2 shadowmapOffset;
  mat4 projectionMatrix;
  float radius;
  uint mask;
};

layout (std140) uniform ProjectorBlock {
  Projector projectors[100];
};

in vec3 vNormal_worldspace;
vec4 ambient = vec4(0.1, 0.1, 0.1, 0.0) * 0.0;

float LinearizeDepth(float depth) {
  vec2 uNearFar = vec2(0.05, 40.0);
  float z = depth * 2.0 - 1.0; // Back to NDC
  return (2.0 * uNearFar.x * uNearFar.y) / (uNearFar.y + uNearFar.x - z * (uNearFar.y - uNearFar.x)) / uNearFar.y;
}

float calculateFragmentShadow(vec2 uv, float fragmentDepth) {
  float shadow = 0.0;
  float bias = 0.001;
  vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
  for(int x = -1; x <= 1; x++) {
    for(int y = -1; y <= 1; y++) {
      float closestDepth = texture(uShadowMap, uv + vec2(x, y) * texelSize).r;
      shadow += fragmentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
  }

  shadow /= 9.0;
  return shadow;
}

vec3 calculateFragmentDiffuse(float normalizedDistanceToLight, float attenuation, vec3 normal, vec3 lightDir, vec3 eyeDir, vec3 lightColor, float materialSpecular) {
  float lightValue = clamp(dot(-lightDir, normal), 0.0, 1.0);
  float attenuationValue = pow(max(1.0 - normalizedDistanceToLight, 0.0), attenuation);
  vec3 diffuse = lightColor * lightValue;

  // TODO: conditionnaly skip specular
  vec3 reflect = reflect(lightDir, normal);
  float cosAlpha = clamp(dot(eyeDir, reflect), 0.0, 1.0);
  vec3 specular = pow(cosAlpha, 32.0) * lightColor * materialSpecular;

  return attenuationValue * (diffuse + specular);
}
