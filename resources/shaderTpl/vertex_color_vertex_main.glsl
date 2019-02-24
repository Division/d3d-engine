  vVertexColor = aVertexColor;
{% if POINT_SIZE %}  gl_PointSize = aVertexColor.a;
  vVertexColor.a = 1.0;{% endif %}
