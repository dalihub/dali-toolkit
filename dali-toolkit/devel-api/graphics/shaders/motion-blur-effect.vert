#version 430

layout( location = 0 ) in vec2 aPosition;

layout( set = 0, binding = 0, std140 ) uniform vertData
{
  uniform mat4 uMvpMatrix;
  uniform mat4 uModelView;
  uniform mat4 uViewMatrix;
  uniform mat4 uProjection;
  uniform vec3 uSize;
  uniform mat4 uModelLastFrame;
  uniform float uGeometryStretchFactor;
  uniform float uSpeedScalingFactor;
};

layout( location = 0 ) out vec2 vModelSpaceCenterToPos;
layout( location = 1 ) out vec2 vScreenSpaceVelocityVector;
layout( location = 2 ) out float vSpeed;
layout( location = 3 ) out vec2 vTexCoord;

float timeDelta = 0.0167;


void main()
{
  // get view space position of vertex this frame and last frame
  vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;

  vec4 viewSpaceVertex = uModelView * vertexPosition;
  vec4 viewSpaceVertexLastFrame = (uViewMatrix * uModelLastFrame) * vertexPosition;
  float reciprocalTimeDelta = 1.0 / timeDelta;

  // work out vertex's last movement in view space
  vec3 viewSpacePosDelta = viewSpaceVertex.xyz - viewSpaceVertexLastFrame.xyz;

  // get clip space position of vertex this frame and last frame
  vec4 clipSpaceVertex = uMvpMatrix * vertexPosition;
  vec4 clipSpaceVertexLastFrame = uProjection * viewSpaceVertexLastFrame;

  // decide how much this vertex is 'trailing', i.e. at the back of the object relative to its direction of motion. We do this
  // by assuming the objects model space origin is at its center and taking the dot product of the vector from center to vertex with the motion direction
  float t = 0.0;
  float posDeltaLength = length(viewSpacePosDelta);
  if(posDeltaLength > 0.001) // avoid div by 0 if object has barely moved
  {
    vec4 viewSpaceCenterToPos = uModelView * vec4(vertexPosition.xy, 0.0, 0.0);
    float centerToVertexDist = length(viewSpaceCenterToPos);
    if(centerToVertexDist > 0.001) // avoid div by 0 if object has vertex at model space origin
    {
      vec3 viewSpacePosDeltaNormalised = viewSpacePosDelta / posDeltaLength;
      vec3 viewSpaceCenterToPosNormalised = viewSpaceCenterToPos.xyz / centerToVertexDist;
      t = (dot(viewSpacePosDeltaNormalised, viewSpaceCenterToPosNormalised) * 0.5 ) + 0.5; // scale and bias from [-1..1] to [0..1]
    }
  }
  // output vertex position lerped with its last position, based on how much it is trailing,
  // this stretches the geom back along where it has just been, giving a warping effect
  // Note: we must take account of time delta to convert position delta into a velocity, so changes are smooth (take into account frame time correctly)
  gl_Position = mix(clipSpaceVertexLastFrame, clipSpaceVertex, t * uGeometryStretchFactor * reciprocalTimeDelta);

  // work out vertex's last movement in normalised device coordinates [-1..1] space, i.e. perspective divide
  vec2 ndcVertex = clipSpaceVertex.xy / clipSpaceVertex.w;
  vec2 ndcVertexLastFrame = clipSpaceVertexLastFrame.xy / clipSpaceVertexLastFrame.w;
  // scale and bias so that a value of 1.0 corresponds to screen size (NDC is [-1..1] = 2)
  vScreenSpaceVelocityVector = ((ndcVertex - ndcVertexLastFrame) * 0.5 * reciprocalTimeDelta);
  vScreenSpaceVelocityVector.y = -vScreenSpaceVelocityVector.y; // TODO negated due to y being inverted in our coordinate system?
  // calculate a scaling factor proportional to velocity, which we can use to tweak how things look
  vSpeed = length(vScreenSpaceVelocityVector) * uSpeedScalingFactor;
  vSpeed = clamp(vSpeed, 0.0, 1.0);

  // provide fragment shader with vector from center of object to pixel (assumes the objects model space origin is at its center and verts have same z)
  vModelSpaceCenterToPos = viewSpaceVertex.xy;

  vec2 texCoord = aPosition + vec2(0.5);
  vTexCoord = texCoord;
}
