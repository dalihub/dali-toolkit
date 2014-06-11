//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/public-api/shader-effects/motion-blur-effect.h>
namespace Dali
{

namespace Toolkit
{

namespace
{

const std::string MOTION_BLUR_TEXCOORD_SCALE_PROPERTY_NAME( "uBlurTexCoordScale" );
const std::string MOTION_BLUR_GEOM_STRETCH_SCALING_FACTOR_PROPERTY_NAME( "uGeometryStretchFactor" );
const std::string MOTION_BLUR_SPEED_SCALING_FACTOR_PROPERTY_NAME( "uSpeedScalingFactor" );
const std::string MOTION_BLUR_OBJECT_FADE_START_PROPERTY_NAME( "uObjectFadeStart" );
const std::string MOTION_BLUR_OBJECT_FADE_END_PROPERTY_NAME( "uObjectFadeEnd" );
const std::string MOTION_BLUR_ALPHA_SCALE_PROPERTY_NAME( "uAlphaScale" );
const std::string MOTION_BLUR_NUM_SAMPLES_NAME( "uNumSamples" );
const std::string MOTION_BLUR_RECIP_NUM_SAMPLES_NAME( "uRecipNumSamples" );
const std::string MOTION_BLUR_RECIP_NUM_SAMPLES_MINUS_ONE_NAME( "uRecipNumSamplesMinusOne" );
const std::string MOTION_BLUR_MODEL_LASTFRAME( "uModelLastFrame" );  ///< Matrix

////////////////////////////////////////////////////
//
// Motion blur shader / actor tweaking parameters
//

const unsigned int MOTION_BLUR_NUM_SAMPLES = 8;

// half width and half height respectively of actor, corresponding to values in vertex attribute stream
// TODO: Note that these values work for normal image actor (verts +/- 0.5) but a grid or a nine square seems to have verts in pixel space (e.g. 256,256). Need to fix this somehow,
// either in Dali or by passing uniforms which we can use to 'normalise' the verts in the vertex shader
const Vector2 MOTION_BLUR_ACTOR_VERTEX( 0.5f, 0.5f );

const float MOTION_BLUR_TEXCOORD_SCALE = 0.125f;             // stretch texture reads along velocity vector, larger number means reads spaced further apart
const float MOTION_BLUR_GEOM_STRETCH_SCALING_FACTOR = 0.05f; // scaling factor for how much to stretch actor geom as it moves
const float MOTION_BLUR_SPEED_SCALING_FACTOR = 0.5f;         // scales the speed, producing a number affecting how much the actor blurs & fades at the edges

const Vector2 MOTION_BLUR_OBJECT_FADE_END( MOTION_BLUR_ACTOR_VERTEX );             // distance from center at which actor fully fades to zero alpha
const Vector2 MOTION_BLUR_OBJECT_FADE_START( MOTION_BLUR_OBJECT_FADE_END * 0.5f ); // distance from center at which actor start to fade from full alpha

const float MOTION_BLUR_ALPHA_SCALE = 0.75f; // global scaler applied to actor alpha as it is blurred + moving

} // namespace


MotionBlurEffect::MotionBlurEffect()
{
}

//Call the Parent copy constructor to add reference to the implementation for this object
MotionBlurEffect::MotionBlurEffect( ShaderEffect handle )
:ShaderEffect( handle )
{
}

MotionBlurEffect::~MotionBlurEffect()
{
}

MotionBlurEffect MotionBlurEffect::Apply( Actor handle )
{
  MotionBlurEffect newEffect = New( MOTION_BLUR_NUM_SAMPLES );
  handle.SetShaderEffect( newEffect );

  Property::Index uModelProperty = newEffect.GetPropertyIndex( MOTION_BLUR_MODEL_LASTFRAME );

  Constraint constraint = Constraint::New<Matrix>( uModelProperty,
                                                   Source( handle, Actor::WORLD_MATRIX ),
                                                   EqualToConstraint() );

  // and set up constraint.
  newEffect.ApplyConstraint( constraint );
  return newEffect;
}

MotionBlurEffect MotionBlurEffect::New()
{
  return New( MOTION_BLUR_NUM_SAMPLES );
}

MotionBlurEffect MotionBlurEffect::New( const unsigned int numBlurSamples )
{
  // Dali vertexSource prefix for reference:
  // precision highp float;
  // attribute vec3  aPosition;
  // attribute vec2  aTexCoord;
  // uniform   mat4  uMvpMatrix;
  // uniform   mat4  uModelView;
  // uniform   mat3  uNormalMatrix;
  // uniform   mat4  uProjection;
  // uniform   vec4  uColor;
  // varying   vec2  vTexCoord;
  std::string vertexSource;
  vertexSource =
    "uniform mat4 uModelLastFrame;\n"
    "uniform float uTimeDelta;\n"

    "uniform float uGeometryStretchFactor;\n"
    "uniform float uSpeedScalingFactor;\n"

    // outputs
    "varying vec2 vModelSpaceCenterToPos;\n"
    "varying vec2 vScreenSpaceVelocityVector;\n"
    "varying float vSpeed;\n"

    "void main()\n"
    "{\n"
    // get view space position of vertex this frame and last frame
    " vec4 vertex = vec4(aPosition, 1.0);\n"
    " vec4 viewSpaceVertex = uModelView * vertex;\n"
    " vec4 viewSpaceVertexLastFrame = (uViewMatrix * uModelLastFrame) * vertex;\n"
    " float reciprocalTimeDelta = 1.0 / ((uTimeDelta > 0.0) ? uTimeDelta : 0.01);\n"

    // work out vertex's last movement in view space
    " vec3 viewSpacePosDelta = viewSpaceVertex.xyz - viewSpaceVertexLastFrame.xyz;\n"

    // get clip space position of vertex this frame and last frame
    " vec4 clipSpaceVertex = uMvpMatrix * vertex;\n"
    " vec4 clipSpaceVertexLastFrame = uProjection * viewSpaceVertexLastFrame;\n"

    // decide how much this vertex is 'trailing', i.e. at the back of the object relative to its direction of motion. We do this
    // by assuming the objects model space origin is at its center and taking the dot product of the vector from center to vertex with the motion direction
    " float t = 0.0;\n"
    " float posDeltaLength = length(viewSpacePosDelta);\n"
    " if(posDeltaLength > 0.001)\n" // avoid div by 0 if object has barely moved
    " {\n"
    "   vec4 viewSpaceCenterToPos = uModelView * vec4(aPosition, 0.0);\n"
    "   float centerToVertexDist = length(viewSpaceCenterToPos);\n"
    "   if(centerToVertexDist > 0.001)\n" // avoid div by 0 if object has vertex at model space origin
    "   {\n"
    "     vec3 viewSpacePosDeltaNormalised = viewSpacePosDelta / posDeltaLength;\n"
    "     vec3 viewSpaceCenterToPosNormalised = viewSpaceCenterToPos.xyz / centerToVertexDist;\n"
    "     t = (dot(viewSpacePosDeltaNormalised, viewSpaceCenterToPosNormalised) * 0.5 ) + 0.5;\n" // scale and bias from [-1..1] to [0..1]
    "   }\n"
    " }\n"
    // output vertex position lerped with its last position, based on how much it is trailing,
    // this stretches the geom back along where it has just been, giving a warping effect
    // Note: we must take account of time delta to convert position delta into a velocity, so changes are smooth (take into account frame time correctly)
    " gl_Position = mix(clipSpaceVertexLastFrame, clipSpaceVertex, t * uGeometryStretchFactor * reciprocalTimeDelta);\n"

    // work out vertex's last movement in normalised device coordinates [-1..1] space, i.e. perspective divide
    " vec2 ndcVertex = clipSpaceVertex.xy / clipSpaceVertex.w;\n"
    " vec2 ndcVertexLastFrame = clipSpaceVertexLastFrame.xy / clipSpaceVertexLastFrame.w;\n"
    // scale and bias so that a value of 1.0 corresponds to screen size (NDC is [-1..1] = 2)
    " vScreenSpaceVelocityVector = ((ndcVertex - ndcVertexLastFrame) * 0.5 * reciprocalTimeDelta);\n"
    " vScreenSpaceVelocityVector.y = -vScreenSpaceVelocityVector.y;\n" // TODO negated due to y being inverted in our coordinate system?
    // calculate a scaling factor proportional to velocity, which we can use to tweak how things look
    " vSpeed = length(vScreenSpaceVelocityVector) * uSpeedScalingFactor;\n"
    " vSpeed = clamp(vSpeed, 0.0, 1.0);\n"

    // provide fragment shader with vector from center of object to pixel (assumes the objects model space origin is at its center and verts have same z)
    " vModelSpaceCenterToPos = aPosition.xy;\n"

    " vTexCoord = aTexCoord;\n"
    "}\n";


  // Dali fragmentSource prefix for reference:
  // precision highp     float;
  // uniform   sampler2D sTexture;
  // uniform   sampler2D sEffect;
  // uniform   vec4      uColor;
  // varying   vec2      vTexCoord;
  std::string fragmentSource;
  fragmentSource =
    "precision mediump float;\n"

    "uniform vec2 uObjectFadeStart;\n"
    "uniform vec2 uObjectFadeEnd;\n"
    "uniform float uAlphaScale;\n"
    "uniform float uBlurTexCoordScale;\n"
    "uniform float uNumSamples;\n"
    "uniform float uRecipNumSamples;\n"
    "uniform float uRecipNumSamplesMinusOne;\n"
    // inputs
    "varying vec2 vModelSpaceCenterToPos;\n"
    "varying vec2 vScreenSpaceVelocityVector;\n"
    "varying float vSpeed;\n"

    "void main()\n"
    "{\n"
    // calculate an alpha value that will fade the object towards its extremities, we need this to avoid an unsightly hard edge between color values of
    // the blurred object and the unblurred background. Use smoothstep also to hide any hard edges (discontinuities) in rate of change of this alpha gradient
    " vec2 centerToPixel = abs(vModelSpaceCenterToPos);\n"
    " vec2 fadeToEdges = smoothstep(0.0, 1.0, 1.0 - ((centerToPixel - uObjectFadeStart) / (uObjectFadeEnd - uObjectFadeStart)));\n"
    " float fadeToEdgesScale = fadeToEdges.x * fadeToEdges.y * uAlphaScale;\n" // apply global scaler
    " fadeToEdgesScale = mix(1.0, fadeToEdgesScale, vSpeed);\n" // fade proportional to speed, so opaque when at rest

    // scale velocity vector by user requirements
    " vec2 velocity = vScreenSpaceVelocityVector * uBlurTexCoordScale;\n"

    // standard actor texel
    " vec4 colActor = texture2D(sTexture, vTexCoord);\n"

    // blurred actor - gather texture samples from the actor texture in the direction of motion
    " vec4 col = colActor * uRecipNumSamples;\n"
    " for(float i = 1.0; i < uNumSamples; i += 1.0)\n"
    " {\n"
    "   float t = i * uRecipNumSamplesMinusOne;\n"
    "   col += texture2D(sTexture, vTexCoord + (velocity * t)) * uRecipNumSamples;\n"
    " }\n"
    " gl_FragColor = mix(colActor, col, vSpeed);\n" // lerp blurred and non-blurred actor based on speed of motion
    " gl_FragColor.a = colActor.a * fadeToEdgesScale;\n" // fade blurred actor to its edges based on speed of motion
    " gl_FragColor *= uColor;\n"
    "}\n";

  // NOTE: we must turn on alpha blending for the actor (HINT_BLENDING)
  ShaderEffect shader = ShaderEffect::New( vertexSource,
                                           fragmentSource,
                                           GEOMETRY_TYPE_IMAGE,
                                           ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING | ShaderEffect::HINT_GRID) );

  MotionBlurEffect handle( shader );


  //////////////////////////////////////
  // Register uniform properties
  //
  //

  // factors that scale the look, defaults
  handle.SetUniform( MOTION_BLUR_TEXCOORD_SCALE_PROPERTY_NAME, MOTION_BLUR_TEXCOORD_SCALE );
  handle.SetUniform( MOTION_BLUR_GEOM_STRETCH_SCALING_FACTOR_PROPERTY_NAME, MOTION_BLUR_GEOM_STRETCH_SCALING_FACTOR );
  handle.SetUniform( MOTION_BLUR_SPEED_SCALING_FACTOR_PROPERTY_NAME, MOTION_BLUR_SPEED_SCALING_FACTOR );
  handle.SetUniform( MOTION_BLUR_OBJECT_FADE_START_PROPERTY_NAME, MOTION_BLUR_OBJECT_FADE_START );
  handle.SetUniform( MOTION_BLUR_OBJECT_FADE_END_PROPERTY_NAME, MOTION_BLUR_OBJECT_FADE_END );
  handle.SetUniform( MOTION_BLUR_ALPHA_SCALE_PROPERTY_NAME, MOTION_BLUR_ALPHA_SCALE );
  handle.SetUniform( MOTION_BLUR_NUM_SAMPLES_NAME, static_cast<float>( MOTION_BLUR_NUM_SAMPLES ) );
  handle.SetUniform( MOTION_BLUR_RECIP_NUM_SAMPLES_NAME, 1.0f / static_cast<float>( MOTION_BLUR_NUM_SAMPLES ) );
  handle.SetUniform( MOTION_BLUR_RECIP_NUM_SAMPLES_MINUS_ONE_NAME, 1.0f / static_cast<float>( MOTION_BLUR_NUM_SAMPLES - 1.0f ) );
  handle.SetUniform( MOTION_BLUR_MODEL_LASTFRAME, Matrix::IDENTITY );

  return handle;
}

MotionBlurEffect MotionBlurEffect::DownCast( ShaderEffect shaderEffect )
{
  MotionBlurEffect handle = shaderEffect;
  return handle;
}

void MotionBlurEffect::SetNumSamples( int numSamples )
{
  SetUniform( MOTION_BLUR_NUM_SAMPLES_NAME, static_cast<float>( numSamples ) );
  SetUniform( MOTION_BLUR_RECIP_NUM_SAMPLES_NAME, 1.0f / static_cast<float>( numSamples ) );
  SetUniform( MOTION_BLUR_RECIP_NUM_SAMPLES_MINUS_ONE_NAME, 1.0f / static_cast<float>( numSamples - 1.0f ) );
}

void MotionBlurEffect::SetTexcoordScale( float texcoordScale )
{
  SetUniform( MOTION_BLUR_TEXCOORD_SCALE_PROPERTY_NAME, texcoordScale );
}

void MotionBlurEffect::SetGeometryStretchFactor( float scalingFactor )
{
  SetUniform( MOTION_BLUR_GEOM_STRETCH_SCALING_FACTOR_PROPERTY_NAME, scalingFactor );
}

void MotionBlurEffect::SetSpeedScalingFactor( float scalingFactor )
{
  SetUniform( MOTION_BLUR_SPEED_SCALING_FACTOR_PROPERTY_NAME, scalingFactor );
}

void MotionBlurEffect::SetObjectFadeStart( Vector2 displacement )
{
  SetUniform( MOTION_BLUR_OBJECT_FADE_START_PROPERTY_NAME, displacement );
}

void MotionBlurEffect::SetObjectFadeEnd( Vector2 displacement )
{
  SetUniform( MOTION_BLUR_OBJECT_FADE_END_PROPERTY_NAME, displacement );
}

void MotionBlurEffect::SetAlphaScale( float alphaScale )
{
  SetUniform( MOTION_BLUR_ALPHA_SCALE_PROPERTY_NAME, alphaScale );
}

const std::string& MotionBlurEffect::GetTexcoordScalePropertyName() const
{
  return MOTION_BLUR_TEXCOORD_SCALE_PROPERTY_NAME;
}

const std::string& MotionBlurEffect::GetGeometryStretchFactorPropertyName() const
{
  return MOTION_BLUR_GEOM_STRETCH_SCALING_FACTOR_PROPERTY_NAME;
}

const std::string& MotionBlurEffect::GetSpeedScalingFactorPropertyName() const
{
  return MOTION_BLUR_SPEED_SCALING_FACTOR_PROPERTY_NAME;
}

const std::string& MotionBlurEffect::GetObjectFadeStartPropertyName() const
{
  return MOTION_BLUR_OBJECT_FADE_START_PROPERTY_NAME;
}

const std::string& MotionBlurEffect::GetObjectFadeEndPropertyName() const
{
  return MOTION_BLUR_OBJECT_FADE_END_PROPERTY_NAME;
}

const std::string& MotionBlurEffect::GetAlphaScalePropertyName() const
{
  return MOTION_BLUR_ALPHA_SCALE_PROPERTY_NAME;
}

}

}

