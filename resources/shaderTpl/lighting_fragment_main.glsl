{% if SPECULAR_MAP %}
  float materialSpecular = texture(uSpecularMap, vTexCoord0).r;
{% else %}
  float materialSpecular = 1.0;
{% endif %}

  float TILE_SIZE = 32.0;
  vec2 screenSize = vec2(camera.screenSize);
  ivec2 tilesCount = ivec2(ceil(screenSize / TILE_SIZE));

  int tileX = int(floor(gl_FragCoord.x / TILE_SIZE));
  int tileY = int(floor(gl_FragCoord.y / TILE_SIZE));
  vec2 tileCount = ceil(vec2(screenSize / TILE_SIZE));

{% if USE_BUFFER_TEXTURE %}
  int tileIndex = tileX + tilesCount.x * tileY;
  uvec4 gridItem = texelFetch(uLightGrid, tileIndex);
{% else %}
  uvec4 gridItem = texture(uLightGrid, vec2(float(tileX) / tileCount.x, float(tileY) / tileCount.y));
{% endif %}

  uint lightOffset = gridItem.r;
  uint pointLightCount = gridItem.g & 0x000fffu;
  uint spotLightCount = gridItem.g >> 16;
  uint projectorCount = gridItem.b & 0x000fffu;
  uint decalCount = gridItem.b >> 16;
  vec3 eyeDir_worldspace = normalize(camera.position - vPosition_worldspace.xyz); // vector to camera
  vec4 lightsColor = vec4(0, 0, 0, 1);
