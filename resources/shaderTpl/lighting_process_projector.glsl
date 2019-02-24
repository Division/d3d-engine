  for (uint i = 0u; i < projectorCount; i++) {
    uint currentOffset = lightOffset + i + pointLightCount + spotLightCount;
    {% if USE_BUFFER_TEXTURE %}
        uint projectorIndex = texelFetch(uLightIndices, int(currentOffset)).r;
    {% else %}
        uint projectorIndex = texelFetch(uLightIndices, ivec2(currentOffset % 4096u, int(floor(float(currentOffset) / 4096.0))), 0).r;
    {% endif %}

    vec4 projectedTextureUV = projectors[projectorIndex].projectionMatrix * vPosition_worldspace;
    projectedTextureUV /= projectedTextureUV.w;
    projectedTextureUV = (projectedTextureUV + 1.0) / 2.0;
    float projectorBias = 0.001;
    if ((projectors[projectorIndex].mask & objectParams.layer) > 0u
        && projectedTextureUV.x > projectorBias && projectedTextureUV.x < 1.0 - projectorBias
        && projectedTextureUV.y > projectorBias && projectedTextureUV.y < 1.0 - projectorBias
        && projectedTextureUV.z > projectorBias && projectedTextureUV.z < 1.0 - projectorBias) {
      vec2 spritesheetUV = projectedTextureUV.xy * projectors[projectorIndex].scale + projectors[projectorIndex].offset;
      vec4 projectedTexture = texture(uProjectorTexture, spritesheetUV) * projectors[projectorIndex].color;

      vec3 lightPosition = projectors[projectorIndex].position;

      vec3 lightDir = vPosition_worldspace.xyz - lightPosition;
      float distanceToLight = length(lightDir);
      lightDir /= distanceToLight; // normalize
      vec3 lightColor = projectedTexture.rgb * projectedTexture.a;
      float normalizedDistanceToLight = distanceToLight / projectors[projectorIndex].radius;
      vec3 lightValue = calculateFragmentDiffuse(normalizedDistanceToLight, projectors[projectorIndex].attenuation, normal_worldspace, lightDir, eyeDir_worldspace, lightColor, materialSpecular);

      if (projectors[projectorIndex].shadowmapScale.x > 0) {
        vec2 shadowmapUV = projectedTextureUV.xy * projectors[projectorIndex].shadowmapScale + projectors[projectorIndex].shadowmapOffset;
        float shadow = calculateFragmentShadow(shadowmapUV, projectedTextureUV.z);
        lightValue *= 1.0 - shadow;
      }

      lightsColor += vec4(lightValue, 0.0);
    }
  }