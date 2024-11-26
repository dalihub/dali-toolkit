attribute mediump vec3 aPosition;
uniform   mediump mat4 uModelView;
uniform   mediump mat4 uModelMatrix;
uniform   mediump mat4 uViewMatrix;
uniform   mediump mat4 uProjection;
uniform   mediump mat4 uMvpMatrix;
varying   mediump vec3 vTexCoord;

void main()
{
  vTexCoord.x = aPosition.x;
  vTexCoord.y = -aPosition.y; // convert to GL coords
  vTexCoord.z = aPosition.z;

  mediump vec4 vertexPosition = vec4(aPosition, 1.0);
  vec4 clipSpacePosition = uProjection * mat4(mat3(uModelView)) * vertexPosition;
  // Writes 1.0, the maximum depth value, into the depth buffer.
  // This is an optimization to avoid running the fragment shader
  // for the pixels hidden by the scene's objects.
  gl_Position = clipSpacePosition.xyww;
}