  for (uint i = 0u; i < pointLightCount; i++) {
    uint currentOffset = lightOffset + i;
{% if USE_BUFFER_TEXTURE %}
    uint lightIndex = texelFetch(uLightIndices, int(currentOffset)).r;
{% else %}
    uint lightIndex = texelFetch(uLightIndices, ivec2(currentOffset % 4096u, int(floor(float(currentOffset) / 4096.0))), 0).r;
{% endif %}
    if ((lights[lightIndex].mask & objectParams.layer) > 0u) {
      vec3 lightPosition = lights[lightIndex].position;
      vec3 lightDir = vPosition_worldspace.xyz - lightPosition;
      float distanceToLight = length(lightDir);
      lightDir /= distanceToLight; // normalize
      float normalizedDistanceToLight = distanceToLight / lights[lightIndex].radius;
      vec3 lightValue = calculateFragmentDiffuse(normalizedDistanceToLight, lights[lightIndex].attenuation, normal_worldspace, lightDir, eyeDir_worldspace, lights[lightIndex].color, materialSpecular);

      vec3 coneDirection = lights[lightIndex].direction;
      float coneAngle = lights[lightIndex].coneAngle;
      float lightToSurfaceAngle = dot(lightDir, coneDirection);
      float innerLightToSurfaceAngle = lightToSurfaceAngle * 1.03;
      float epsilon = innerLightToSurfaceAngle - lightToSurfaceAngle;

      if (lightToSurfaceAngle > coneAngle && lights[lightIndex].shadowmapScale.x > 0) {
        vec4 position_lightspace = lights[lightIndex].projectionMatrix * vPosition_worldspace;
        vec4 position_lightspace_normalized = position_lightspace / position_lightspace.w;
        position_lightspace_normalized = (position_lightspace_normalized + 1.0) / 2.0;
        vec2 shadowmapUV = position_lightspace_normalized.xy * lights[lightIndex].shadowmapScale + lights[lightIndex].shadowmapOffset;
        float shadow = calculateFragmentShadow(shadowmapUV, position_lightspace_normalized.z);
        lightValue *= 1.0 - shadow;
      }

      lightsColor += vec4(lightValue, 0.0);

    }
  }