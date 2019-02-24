// TERRAIN ///////////////////////////////////////
{% if MODE == "FRAGMENT_UNIFORM_DECLARE" %}

{% if TERRAIN_LAYER0 %}uniform highp sampler2D uTerrainDiffuse0;
{% if NORMAL_MAP %}uniform highp sampler2D uTerrainNormal0;{% endif %}
{% endif %}

{% if TERRAIN_LAYER1 %}uniform highp sampler2D uTerrainDiffuse1;
{% if NORMAL_MAP %}uniform highp sampler2D uTerrainNormal1;{% endif %}
uniform highp sampler2D uTerrainSplatmap;
{% endif %}

{% if TERRAIN_LAYER2 %}uniform highp sampler2D uTerrainDiffuse2;
{% if NORMAL_MAP %}uniform highp sampler2D uTerrainNormal2;{% endif %}
{% endif %}

{% endif %}

{% if MODE == "FRAGMENT_MAIN" %}
  vec4 terrainDiffuse0 = texture(uTerrainDiffuse0, vTexCoord0);
{% if SPECULAR_MAP %}  vec4 terrainSpecularMap = texture(uSpecularMap, vTexCoord0 / 20.0f);{% endif %}

{% if NORMAL_MAP %}  vec4 terrainNormal0 = texture(uTerrainNormal0, vTexCoord0);{% endif %}
{% if TERRAIN_LAYER1 %}  vec4 terrainDiffuse1 = texture(uTerrainDiffuse1, vTexCoord0);
  vec4 terrainSplatmap = texture(uTerrainSplatmap, vTexCoord0 / 20.0f);
{% if NORMAL_MAP %}  vec4 terrainNormal1 = texture(uTerrainNormal1, vTexCoord0);{% endif %}
{% endif %}

{% if TERRAIN_LAYER2 %}  vec4 terrainDiffuse2 = texture(uTerrainDiffuse2, vTexCoord0);
{% if NORMAL_MAP %}  vec4 terrainNormal2 = texture(uTerrainNormal2, vTexCoord0);{% endif %}
{% endif %}

{% if TERRAIN_LAYER2 %}

{% if NORMAL_MAP %}  vec3 normal_tangentspace = terrainNormal0.rgb * terrainSplatmap.r + terrainNormal1.rgb * terrainSplatmap.g + terrainNormal2.rgb * terrainSplatmap.b;{% endif %}
{% if SPECULAR_MAP %}  materialSpecular = terrainSpecularMap.r * terrainSplatmap.r + terrainSpecularMap.g * terrainSplatmap.g + terrainSpecularMap.b * terrainSplatmap.b;{% endif %}
  fragmentColor *= terrainDiffuse0 * terrainSplatmap.r + terrainDiffuse1 * terrainSplatmap.g + terrainDiffuse2 * terrainSplatmap.b;
{% else if TERRAIN_LAYER1 %}
{% if NORMAL_MAP %}  vec3 normal_tangentspace = terrainNormal0.rgb * terrainSplatmap.r + terrainNormal1.rgb * terrainSplatmap.g;{% endif %}
{% if SPECULAR_MAP %}  materialSpecular = terrainSpecularMap.r * terrainSplatmap.r + terrainSpecularMap.g * terrainSplatmap.g;{% endif %}
  fragmentColor *= terrainDiffuse0 * terrainSplatmap.r + terrainDiffuse1 * terrainSplatmap.g;
{% else %}
{% if NORMAL_MAP %}  vec3 normal_tangentspace = terrainNormal0.rgb;{% endif %}
{% if SPECULAR_MAP %}  materialSpecular = terrainSpecularMap.r;{% endif %}
  fragmentColor *= terrainDiffuse0;
{% endif %}


{% if NORMAL_MAP %}
//normal_tangentspace = vec3(0.5, 0.5, 1);
normal_tangentspace = normal_tangentspace * 2.0 - 1.0;
  // Need inverted z for correct lighting
  //normal_tangentspace.z *= -1.0;{% endif %}
{% endif %}

// END TERRAIN ////////////////////////////////////