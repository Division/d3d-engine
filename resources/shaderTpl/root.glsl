[vertex]
#version {{ version }}

{{ object_params("") }}

{{ camera("") }}

{% if LIGHTING %}{{ lighting_vertex("") }}{% endif %}

{% if SKINNING %}{{ skinning_vertex("") }}{% endif %}

{% if NORMAL_MAP %}{{ normal_mapping_vertex("") }}{% endif %}

{% if VERTEX_COLOR %}{{ vertex_color_vertex("") }}{% endif %}

in vec3 aPosition;
in vec2 aTexCoord0;
{% if BILLBOARD %}in vec2 aCorner;{% endif %}
out vec2 vTexCoord0;
out vec4 vPosition_worldspace;

void main(void) {
  vTexCoord0 = aTexCoord0 * objectParams.uvScale + objectParams.uvOffset;

{% if VERTEX_COLOR %}{{ vertex_color_vertex_main("") }}{% endif %}

  mat4 modelMatrix;
{% if SKINNING %}
  {{ skinning_vertex_main("") }}
{% else %}
  modelMatrix = objectParams.model;
{% endif %}

  vPosition_worldspace = modelMatrix * vec4(aPosition, 1.0);
{% if BILLBOARD %}
  vPosition_worldspace += aCorner.x * vec4(camera.viewMatrix[0][0], camera.viewMatrix[1][0], camera.viewMatrix[2][0], 0.0) +
                          aCorner.y * vec4(camera.viewMatrix[0][1], camera.viewMatrix[1][1], camera.viewMatrix[2][1], 0.0);

{% endif %}

  vec4 position_cameraspace = camera.viewMatrix * vPosition_worldspace;

{% if LIGHTING %}{{ lighting_vertex_main("") }}{% endif %}

{% if NORMAL_MAP %}{{ normal_mapping_vertex_main("") }}{% endif %}

  gl_Position = camera.projectionMatrix * position_cameraspace;
}

[fragment]
#version {{ version }}
{% if WEBGL %}precision highp float; precision highp int;{% endif %}
out vec4 fragmentColor;

{{ object_params("") }}

{{ camera("") }}

{% if VERTEX_COLOR %}{{ vertex_color_fragment("") }}{% endif %}

in vec2 vTexCoord0;
{% if TEXTURE0 %}{{ texture_fragment("") }}{% endif %}

{% if COLOR %}uniform vec4 uColor;{% endif %}
in vec4 vPosition_worldspace;

{% if PROJECTED_TEXTURE %}{{ projectedTexture("FRAGMENT_UNIFORM_DECLARE") }}{% endif %}
{% if TERRAIN_LAYER0 %}{{ terrain("FRAGMENT_UNIFORM_DECLARE") }}{% endif %}

{% if LIGHTING %}{{ lighting_fragment("") }}{% endif %}

{% if NORMAL_MAP %}{{ normal_mapping_fragment("") }}{% endif %}

void main(void) {
{% if COLOR %}
  fragmentColor = uColor;
{% else %}
  fragmentColor = vec4(1.0, 1.0, 1.0, 1.0);
{% endif %}

{% if VERTEX_COLOR %}{{ vertex_color_fragment_main("") }}{% endif %}
{% if TERRAIN_LAYER0 %}{{ terrain("FRAGMENT_MAIN") }}{% endif %}
{% if PROJECTED_TEXTURE %}{{ projectedTexture("FRAGMENT_MAIN") }}{% endif %}

{% if NORMAL_MAP %}
{{ normal_mapping_fragment_main("") }}
{% else %}
{% if LIGHTING %}  vec3 normal_worldspace = normalize(vNormal_worldspace);{% endif %}
{% endif %}

{% if LIGHTING %}
{{ lighting_fragment_main("") }}
{{ lighting_process_point("") }}
{{ lighting_process_spot("") }}
{{ lighting_process_projector("") }}
{{ lighting_process_decal("") }}
  lightsColor += ambient;
  fragmentColor *= lightsColor + ambient;
{% endif %}

{% if TEXTURE0 %}{{ texture_fragment_main("") }}{% endif %}

// Gamma correction
  float gamma = 2.2;
  fragmentColor.rgb = pow(fragmentColor.rgb, vec3(1.0/gamma));
}
