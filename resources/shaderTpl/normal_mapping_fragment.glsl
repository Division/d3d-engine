in vec3 vTangent_worldspace;
in vec3 vBitangent_worldspace;
{% if not TERRAIN_LAYER0 %}
uniform highp sampler2D uNormalMap;
{% endif %}
