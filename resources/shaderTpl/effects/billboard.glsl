[vertex]
#version {{ version }}
in vec3 aPosition;
in vec2 aTexCoord0;
out vec2 vTexCoord0;

void main()
{
  gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
  vTexCoord0 = aTexCoord0;
}

[fragment]
#version {{ version }}

in vec2 vTexCoord0;
out vec4 fragmentColor;

uniform highp sampler2D uTexture0;

float LinearizeDepth(float depth)
{
  return depth * 2.0 - 1.0; // Back to NDC
}

void main()
{
  float depthValue = texture(uTexture0, vTexCoord0).r;
  fragmentColor = vec4(vec3(depthValue), 1.0);
}
