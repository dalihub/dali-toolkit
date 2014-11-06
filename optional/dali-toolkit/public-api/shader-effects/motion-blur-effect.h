#ifndef __DALI_TOOLKIT_SHADER_EFFECT_MOTION_BLUR_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_MOTION_BLUR_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/renderable-actor.h>
#include <dali/public-api/shader-effects/shader-effect.h>

namespace Dali
{

namespace Toolkit
{

/**
 *
 * Class for motion blur shader that works on a per object basis. Objects will
 * blur when they move, or if the camera moves. Can be applied to ImageActor or
 * TextActor only.
 *
 * Usage example:-
 *
 * // Create shader used for doing motion blur\n
 * MotionBlurEffect MotionBlurEffect = MotionBlurEffect::New();
 *
 * // set actor shader to the blur one\n
 * Actor Actor = Actor::New( ... );\n
 * Actor.SetShaderEffect( MotionBlurEffect );
 *
 */
class DALI_IMPORT_API MotionBlurEffect : public ShaderEffect
{

public:

  /**
   * Create an uninitialized MotionBlurEffect; this can be initialized with MotionBlurEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  MotionBlurEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~MotionBlurEffect();

  /**
   * Create an initialized MotionBlurEffect
   * The number of texture samples taken along the motion velocity vector of the
   * actor, producing the blur, is set to a default of 8.
   * @return A handle to a newly allocated Dali resource.
   */
  static MotionBlurEffect New();

  /**
   * Create a  MotionBlurEffect and attach it to the specified actor
   * The number of texture samples taken along the motion velocity vector of the
   * actor, producing the blur, is set to a default of 8.
   * @param renderable actor to apply the effect to
   * @return A handle to a newly allocated Dali resource.
   */
  static MotionBlurEffect Apply( RenderableActor renderable );

  /**
   * Create an initialized MotionBlurEffect
   * @param numBlurSamples The number of texture samples taken along the motion
   * velocity vector of the actor, producing the blur. A higher number gives a
   * smoother blur but costs more in terms of performance.
   * @param numBlurSamples to have
   * @return A handle to a newly allocated Dali resource.
   */
  static MotionBlurEffect New( unsigned int numBlurSamples );

  /**
   * Set texcoord scale property. This scales the offset for texture samples
   * along the motion velocity vector. A smaller value means the samples will
   * be spaced closer, larger value further apart. User should use this to get
   * the blur to look contiguous, i.e. the blur texels should not be too widely
   * spread, with gaps in between. Default 0.125.
   * @param texcoordScale The scaling factor that multiplies the motion velocity vector for texture lookups.
   */
  void SetTexcoordScale( float texcoordScale );

  /**
   * Set geometry stretch factor property. This scales the amount the
   * geometry stretches backwards along the motion velocity vector. A smaller
   * value means the geometry stretches less, larger it stretches more. User
   * should use this to get the blur to 'bleed' into areas outside the physical
   * bounds of the actor. We need this as the blur is only applied inside the
   * bounds of the actor, but you would expect motion blur trails where the
   * actor was previously but is there no longer. Default 0.05.
   * @param scalingFactor The scaling factor that extrudes the geometry backwards along the motion velocity vector.
   */
  void SetGeometryStretchFactor( float scalingFactor );

  /**
   * Set speed scaling factor property. This takes the magnitude of the motion
   * velocity vector and scales it to produce a value which is used to fade the
   * blur in / out with the speed that the actor is moving. As the blur fades
   * in, more of the blur is visible and less of the original actor, and vice
   * versa.
   * This value is also used to control how much to fade the actor near the
   * edges, based on the speed the actor is moving. When the actor is at rest
   * this is not applied. Default 0.5.
   * @param scalingFactor The scaling factor that controls the edge fade / blur fade of the actor.
   */
  void SetSpeedScalingFactor( float scalingFactor );

  /**
   * Set the displacement from the centre of the actor that the actor will start
   * to fade towards its edges. This is used to prevent an unsightly hard edge
   * between the blurred actor and the scene. Depends on the values of the
   * vertices in the vertex stream. When the actor is at rest this is not applied.
   * Default 0.25, which is half way towards the edge for an ImageRenderer::QUAD.
   * @param displacement The displacement from the centre of the actor that the actor will start to edge fade.
   */
  void SetObjectFadeStart( Vector2 displacement );

  /**
   * Set the displacement from the centre of the actor that the actor will
   * finish fading towards its edges. This is used to prevent an unsightly hard
   * edge between the blurred actor and the scene. Depends on the values of the
   * vertices in the vertex stream. When the actor is at rest this is not applied.
   * Default 0.5, which is all the way towards the edge for an ImageRenderer::QUAD.
   * @param displacement The displacement from the centre of the actor that the actor will finish edge fading.
   */
  void SetObjectFadeEnd( Vector2 displacement );

  /**
   * Set a global scaler applied to the alpha of the actor. Used to make the
   * blurred actor a bit more subtle (helps to hide discontinuities due to
   * limited number of texture samples) and reveal a bit of the background
   * behind it as it moves. When the actor is at rest this is not applied. Default 0.75.
   * @param alphaScale The scaling factor which multiplies the alpha of each pixel of the actor.
   */
  void SetAlphaScale( float alphaScale );

  /**
   * Set the number of texture samples to be taken. Increasing the number of
   * samples provides better quality at the cost of performance.
   * @param numSamples The number of texture samples to be taken. Default is 8.
   */
  void SetNumSamples( int numSamples );

  /**
   * Get the name for the texcoord scale property. Useful for animation.
   * @return A std::string containing the property name
   */
  const std::string& GetTexcoordScalePropertyName() const;

  /**
   * Get the name for the geometry stretching property. Useful for animation.
   * @return A std::string containing the property name
   */
  const std::string& GetGeometryStretchFactorPropertyName() const;

  /**
   * Get the name for the speed scaling property. Useful for animation.
   * @return A std::string containing the property name
   */
  const std::string& GetSpeedScalingFactorPropertyName() const;

  /**
   * Get the name for the fade start property. Useful for animation.
   * @return A std::string containing the property name
   */
  const std::string& GetObjectFadeStartPropertyName() const;

  /**
   * Get the name for the fade end property. Useful for animation.
   * @return A std::string containing the property name
   */
  const std::string& GetObjectFadeEndPropertyName() const;

  /**
   * Get the name for the alpha scale property. Useful for animation.
   * @return A std::string containing the property name
   */
  const std::string& GetAlphaScalePropertyName() const;

  /**
   * Downcast an ShaderEffect handle to MotionBlurEffect handle. If handle points to a MotionBlurEffect object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to a ShaderEffect
   * @return handle to a MotionBlurEffect object or an uninitialized handle
   */
  static MotionBlurEffect DownCast( ShaderEffect handle );

private:
  // Not intended for application developers
  DALI_INTERNAL MotionBlurEffect( ShaderEffect handle );
};

}

}

#endif //#ifndef __DALI_TOOLKIT_SHADER_EFFECT_MOTION_BLUR_H__

