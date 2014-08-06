#ifndef __DALI_TOOLKIT_SHADER_EFFECT_MOTION_STRETCH_H__
#define __DALI_TOOLKIT_SHADER_EFFECT_MOTION_STRETCH_H__

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

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

/**
 *
 * Class for motion stretch shader that works on a per object basis. Objects will stretch in the direction of motion when they move, or if the camera moves. Can be applied
 * to ImageActor or TextActor only.
 *
 * Usage example:-
 *
 * // Create shader used for doing motion stretch\n
 * MotionStretchEffect MotionStretchEffect = MotionStretchEffect::New();
 *
 * // set actor shader to the stretch one\n
 * Actor Actor = Actor::New( ... );\n
 * Actor.SetShaderEffect( MotionStretchEffect );
 *
 */
class MotionStretchEffect : public ShaderEffect
{

public:

  /**
   * Create an uninitialized MotionStretchEffect; this can be initialized with MotionStretchEffect::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  MotionStretchEffect();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~MotionStretchEffect();

  /**
   * Create an initialized MotionStretchEffect
   * @return A handle to a newly allocated Dali resource.
   */
  static MotionStretchEffect New();

  /**
   * Create a  MotionStretchEffect and attach it to the specified actor
   * @param renderable actor to apply the effect to
   * @return A handle to a newly allocated Dali resource.
   */
  static MotionStretchEffect Apply( RenderableActor renderable );

  /**
   * Set geometry stretch factor property. This scales the amount the geometry
   * stretches along the motion velocity vector. A smaller value means the geometry
   * stretches less, larger it stretches more. Default 0.5.
   * @param scalingFactor The scaling factor that extrudes the geometry forwards along the motion velocity vector.
   */
  void SetGeometryStretchFactor( float scalingFactor );

  /**
   * Set speed scaling factor property. This value is used to control how much
   * to fade the actor near the edges, based on the speed the actor is moving.
   * When the actor is at rest this is not applied. Default 0.5.
   * @param scalingFactor The scaling factor that controls the edge fade of the actor.
   */
  void SetSpeedScalingFactor( float scalingFactor );

  /**
   * Set the displacement from the centre of the actor that the actor will start to
   * fade towards its edges. This is used to prevent an unsightly hard edge
   * between the stretched actor and the scene. Depends on the values of the
   * vertices in the vertex stream. When the actor is at rest this is not applied.
   * Default Vector2(0.25, 0.25), which is half way towards the edge for an ImageRenderer::QUAD.
   * @param displacement The displacement from the centre of the actor that the actor will start to edge fade.
   */
  void SetObjectFadeStart( Vector2 displacement );

  /**
   * Set the displacement from the centre of the actor that the actor will
   * finish fading towards its edges. This is used to prevent an unsightly hard
   * edge between the stretched actor and the scene. Depends on the values of
   * the vertices in the vertex stream. When the actor is at rest this is not applied.
   * Default 0.5, which is all the way towards the edge for an ImageRenderer::QUAD.
   * @param displacement The displacement from the centre of the actor that the actor will finish edge fading.
   */
  void SetObjectFadeEnd( Vector2 displacement );

  /**
   * Set a global scaler applied to the alpha of the actor. Used to make the
   * stretched actor a bit more subtle and reveal a bit of the background behind
   * it as it moves. When the actor is at rest this is not applied. Default 0.75.
   * @param alphaScale The scaling factor which multiplies the alpha of each pixel of the actor.
   */
  void SetAlphaScale( float alphaScale );


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
   * Get the name for the fade start X property. Useful for animation.
   * @return A std::string containing the property name
   */
  const std::string& GetObjectFadeStartPropertyName() const;

  /**
   * Get the name for the fade end X property. Useful for animation.
   * @return A std::string containing the property name
   */
  const std::string& GetObjectFadeEndPropertyName() const;

  /**
   * Get the name for the alpha scale property. Useful for animation.
   * @return A std::string containing the property name
   */
  const std::string& GetAlphaScalePropertyName() const;

private:
  // Not intended for application developers
  MotionStretchEffect( ShaderEffect handle );
};

}

}

#endif //#ifndef __DALI_TOOLKIT_SHADER_EFFECT_MOTION_STRETCH_H__

