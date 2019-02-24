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

void main()
{
  vec4 texture = texture(uTexture0, vTexCoord0);
  // float average = 0.2126 * texture.r + 0.7152 * texture.g + 0.0722 * texture.b;
  // fragmentColor = vec4(average, average, average, 1.0);
  // fragmentColor = vec4(vec3(1,1,1) - texture.rgb, 1.0);
  fragmentColor = vec4(texture.rgb, 1.0);
}
