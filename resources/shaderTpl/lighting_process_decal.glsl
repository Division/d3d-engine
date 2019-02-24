  for (uint i = 0u; i < decalCount; i++) {
    uint currentOffset = lightOffset + i + pointLightCount + spotLightCount + projectorCount;
    {% if USE_BUFFER_TEXTURE %}
        uint projectorIndex = texelFetch(uLightIndices, int(currentOffset)).r;
    {% else %}
        uint projectorIndex = texelFetch(uLightIndices, ivec2(currentOffset % 4096u, int(floor(float(currentOffset) / 4096.0))), 0).r;
    {% endif %}

    vec4 projectedTextureUV = projectors[projectorIndex].projectionMatrix * vPosition_worldspace;
    projectedTextureUV /= projectedTextureUV.w;
    projectedTextureUV = (projectedTextureUV + 1.0) / 2.0;
    if ((projectors[projectorIndex].mask & objectParams.layer) > 0u
        && projectedTextureUV.x >= 0.0 && projectedTextureUV.x < 1.0
        && projectedTextureUV.y >= 0.0 && projectedTextureUV.y < 1.0
        && projectedTextureUV.z >= 0.0 && projectedTextureUV.z < 1.0) {
      vec2 spritesheetUV = projectedTextureUV.xy * projectors[projectorIndex].scale + projectors[projectorIndex].offset;
      vec4 projectedTexture = texture(uProjectorTexture, spritesheetUV) * projectors[projectorIndex].color;
      fragmentColor = vec4(mix(fragmentColor.rgb, projectedTexture.rgb, projectedTexture.a), fragmentColor.a);
    }
  }