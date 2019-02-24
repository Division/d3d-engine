  mat3 TBN = mat3(
    vTangent_worldspace,
    vBitangent_worldspace,
    vNormal_worldspace
  );

  {% if not TERRAIN_LAYER0 %}
  vec3 normal_tangentspace = texture(uNormalMap, vTexCoord0).rgb * 2.0 - 1.0;
  {% endif %}

  // normal_tangentspace should be defined BEFORE (fetch the normal map sampler)
  vec3 normal_worldspace = normalize(TBN * normal_tangentspace);