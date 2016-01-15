#ifndef __DALI_TOOLKIT_SHADER_EFFECT_MOTION_STRETCH_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_MOTION_STRETCH_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/actors/image-actor.h>
#include <dali/public-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 * @brief Set the properties for the motion stretch
 */
inline void SetMotionStretchProperties( Actor& actor )
{
  actor.RegisterProperty( "uGeometryStretchFactor", 0.5f );
  actor.RegisterProperty( "uSpeedScalingFactor", 0.5f );
  actor.RegisterProperty( "uObjectFadeStart", Vector2( 0.25f, 0.25f ) );
  actor.RegisterProperty( "uObjectFadeEnd", Vector2( 0.5f, 0.5f ) );
  actor.RegisterProperty( "uAlphaScale", 0.75f );
  Property::Index uModelProperty = actor.RegisterProperty( "uModelLastFrame", Matrix::IDENTITY );

  Constraint constraint = Constraint::New<Matrix>( actor, uModelProperty, EqualToConstraint() );
  constraint.AddSource( Source( actor , Actor::Property::WORLD_MATRIX ) );
  constraint.Apply();
}

/**
 * @brief Creates a new MotionStretchEffect
 *
 * Motion stretch shader works on a per object basis. Objects will stretch in the direction of motion when they move, or if the camera moves.
 *
 * Animatable/Constrainable uniforms:
 *  "uGeometryStretchFactor"  - This scales the amount the geometry stretches along the motion velocity vector.
 *                              A smaller value means the geometry stretches less, larger it stretches more. Default 0.5.
 *  "uSpeedScalingFactor"     - This value is used to control how much to fade the actor near the edges, based on the
 *                              speed the actor is moving. When the actor is at rest this is not applied. Default 0.5.
 *  "uObjectFadeStart"        - The displacement from the centre of the actor that the actor will start to fade towards
 *                              its edges. This is used to prevent an unsightly hard edge between the stretched actor and
 *                              the scene. Depends on the values of the vertices in the vertex stream. When the actor is at
 *                              rest this is not applied. Default Vector2(0.25, 0.25), which is half way towards the edge for
 *                              an ImageRenderer::QUAD.
 *  "uObjectFadeEnd"          - The displacement from the centre of the actor that the actor will finish fading towards its edges.
 *                              This is used to prevent an unsightly hard edge between the stretched actor and the scene. Depends
 *                              on the values of the vertices in the vertex stream. When the actor is at rest this is not applied.
 *                              Default 0.5, which is all the way towards the edge for an ImageRenderer::QUAD.
 *  "uAlphaScale"             - Global scaler applied to the alpha of the actor. Used to make the stretched actor a bit more subtle
 *                              and reveal a bit of the background behind it as it moves. When the actor is at rest this is not
 *                              applied. Default 0.75.
 *  "uModelLastFrame"         - The model to world space transformation matrix of the actor in the previous frame.
 *
 * @return The newly created Property::Map with the motion stretch effect
 */
inline Property::Map CreateMotionStretchEffect()
{
  std::string vertexSource;
  vertexSource =
      "precision mediump float;\n"

      "attribute vec2 aPosition;\n"

      "uniform mat4 uMvpMatrix;\n"
      "uniform mat4 uModelView;\n"
      "uniform mat4 uViewMatrix;\n"
      "uniform mat4 uProjection;\n"
      "uniform vec3 uSize;\n"

      "uniform mat4  uModelLastFrame;\n"
      "float timeDelta = 0.0167;\n"

      "uniform float uGeometryStretchFactor;\n"
      "uniform float uSpeedScalingFactor;\n"

      // outputs
      "varying vec2 vModelSpaceCenterToPos;\n"
      "varying vec2 vScreenSpaceVelocityVector;\n"
      "varying float vSpeed;\n"
      "varying vec2 vTexCoord;\n"

      "void main()\n"
      "{\n"
      // get view space position of vertex this frame and last frame
      " vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);\n"
      " vertexPosition.xyz *= uSize;\n"

      " vec4 viewSpaceVertex = uModelView * vertexPosition;\n"
      " vec4 viewSpaceVertexLastFrame = uViewMatrix * uModelLastFrame * vertexPosition;\n"

      // work out vertex's last movement in view space
      " vec3 viewSpacePosDelta = viewSpaceVertex.xyz - viewSpaceVertexLastFrame.xyz;\n"
      " float reciprocalTimeDelta = 1.0 / timeDelta;\n"

      // get clip space position of vertex this frame and last frame
      " vec4 clipSpaceVertex = uMvpMatrix * vertexPosition;\n"
      " vec4 clipSpaceVertexLastFrame = uProjection * viewSpaceVertexLastFrame;\n"

      // decide how much this vertex is 'trailing', i.e. at the back of the object relative to its direction of motion. We do this
      // by assuming the objects model space origin is at its center and taking the dot product of the vector from center to vertex with the motion direction
      " float t = 0.0;\n"
      " float posDeltaLength = length(viewSpacePosDelta);\n"
      " if(posDeltaLength > 0.001)\n" // avoid div by 0 if object has barely moved
      " {\n"
      "   vec4 viewSpaceCenterToPos = uModelView * vec4(aPosition, 0.0, 0.0);\n"
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
      // We raise t to a power in order that non-trailing vertices are effected much more than trailing ones
      // Note: we must take account of time delta to convert position delta into a velocity, so changes are smooth (take into account frame time correctly)
      " gl_Position = mix(clipSpaceVertexLastFrame, clipSpaceVertex, t * t * t * uGeometryStretchFactor * reciprocalTimeDelta);\n"

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
      " vModelSpaceCenterToPos = viewSpaceVertex.xy;\n"

      " vec2 texCoord = aPosition + vec2(0.5);"
      " vTexCoord = texCoord;\n"
      "}\n";

  std::string fragmentSource;
  fragmentSource =
      "precision mediump float;\n"

      "uniform sampler2D sTexture;\n"
      "uniform vec4 uColor;\n"

      "uniform vec2 uObjectFadeStart;\n"
      "uniform vec2 uObjectFadeEnd;\n"
      "uniform float uAlphaScale;\n"

      // inputs
      "varying vec2 vModelSpaceCenterToPos;\n"
      "varying vec2 vScreenSpaceVelocityVector;\n"
      "varying float vSpeed;\n"
      "varying vec2 vTexCoord;\n"

      "void main()\n"
      "{\n"
      // calculate an alpha value that will fade the object towards its extremities, we need this to avoid an unsightly hard edge between color values of
      // the stretched object and the background. Use smoothstep also to hide any hard edges (discontinuities) in rate of change of this alpha gradient
      " vec2 centerToPixel = abs( vModelSpaceCenterToPos );\n"
      " vec2 fadeToEdges = smoothstep(0.0, 1.0, 1.0 - ((centerToPixel - uObjectFadeStart) / (uObjectFadeEnd - uObjectFadeStart)));\n"
      " float fadeToEdgesScale = fadeToEdges.x * fadeToEdges.y * uAlphaScale;\n" // apply global scaler
      " fadeToEdgesScale = mix(1.0, fadeToEdgesScale, vSpeed);\n" // fade proportional to speed, so opaque when at rest

      // standard actor texel
      " vec4 colActor = texture2D(sTexture, vTexCoord);\n"
      " gl_FragColor = colActor;\n"
      " gl_FragColor.a *= fadeToEdgesScale;\n" // fade actor to its edges based on speed of motion
      " gl_FragColor *= uColor;\n"
      "}";

  Property::Map map;

  Property::Map customShader;
  customShader[ "vertexShader" ] = vertexSource;
  customShader[ "fragmentShader" ] = fragmentSource;

  customShader[ "subdivideGridX" ] = 10;
  customShader[ "subdivideGridY" ] = 10;

  // NOTE: we must turn on alpha blending for the actor (HINT_BLENDING)
  customShader[ "hints" ] = "outputIsTransparent";

  map[ "shader" ] = customShader;
  return map;
}

}

}

#endif //#ifndef __DALI_TOOLKIT_SHADER_EFFECT_MOTION_STRETCH_H__
