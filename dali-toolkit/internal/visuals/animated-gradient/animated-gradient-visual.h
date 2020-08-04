#ifndef DALI_TOOLKIT_INTERNAL_ANIMATED_GRADIENT_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_ANIMATED_GRADIENT_VISUAL_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

//EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali-toolkit/devel-api/visuals/animated-gradient-visual-properties-devel.h>

//INTERNAL INCLUDES
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/scripting/scripting.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
class AnimatedGradientVisual;
typedef IntrusivePtr< AnimatedGradientVisual > AnimatedGradientVisualPtr;

/**
 * This visual which renders smooth transition of colors to the control's quad.
 * There are two types of properties: non-animated property and animated property
 *
 * The following properties are non-animated property.
 *
 * | %Property Name | Type         | Default                   |
 * |----------------|--------------|---------------------------|
 * | gradientType   | GradientType | Linear                    |
 * | unitType       | UnitType     | Object bounding box       |
 * | spreadType     | SpreadType   | Reflect                   |
 *
 * The following properties are animated property.
 *
 * | %Property Name | Type                                     | Default                         |
 * |----------------|------------------------------------------|---------------------------------|
 * | startPosition  | Vector2 or AnimationParameter< Vector2 > | (-0.5, 0)                       |
 * | startColor     | Vector4 or AnimationParameter< Vector4 > | (143., 170., 220., 255.) / 255. |
 * | endPosition    | Vector2 or AnimationParameter< Vector2 > | (0.5, 0)                        |
 * | endColor       | Vector4 or AnimationParameter< Vector4 > | (255., 163., 163., 255.) / 255. |
 * | rotateCenter   | Vector2 or AnimationParameter< Vector2 > | (0.0, 0.0)                      |
 * | rotateAmount   | Float   or AnimationParameter< Float >   | 0.0                             |
 * | offset         | Float   or AnimationParameter< Float >   | (explain details below)         |
 *
 * Each animated property can contain follow AnimationParameter
 *
 * | %AnimationParameter<T>::Propery Name | Type          | Default  |
 * |--------------------------------------|---------------|----------|
 * | start                                | T             | Zero     |
 * | target                               | T             | Zero     |
 * | direction                            | DirectionType | Forward  |
 * | duration                             | Float         | 3.0      |
 * | delay                                | Float         | 0.0      |
 * | repeat                               | Integer       | 0        |
 * | repeat_delay                         | Float         | 0.0      |
 * | motion_type                          | MotionType    | Loop     |
 * | easing_type                          | EasingType    | Linear   |
 *
 * T is animated property value's type. If property type is AnimationParameter< Vector2 >, start and target type is Vector2.
 *
 *
 * gradientType decide the form of gradient
 * unitType decide the coordinate of all positions
 * spreadType decide how to make color where gradient_point is not 0 ~ 1
 * visualSize decide this visual's size hardly. only use when unitType is SCREEN
 *
 * startPoint and startColor decide position and color where gradient_point = 0. if gradientType is RADIAL, here is center of circle.
 * endPoint and endColor  decide position and color where gradient_point = 1.
 * rotateCenter and rotateAmount are same job as its name
 * rotateAmount is radian value
 *
 * offset is main feature of this visual.
 * Image the points which has same gradient_point values. If gradientType is LINEAR, this form is line. If gradientTYPE IS RADIAL, this form is circle.
 * without think about offset value, gradient_point = t color will be like this
 *
 *  color(t) = startColor * (1-t) + endColor * t (0 <= t <= 1)
 *
 * so color be smooth changed when gradient_point change smooth.
 * offset value change the color of gradient_point = t like this
 *
 *  realColor(t) = color(t + offset)
 *
 * so If offset value increas (or decrease) gradient looks like "Flowing" effect
 * default offset value is an unlimited simple loop animation from 0 to 2. duration is 3.0 second
 *
 * GradientType has two types : LINEAR / RADIAL
 *  LINEAR draw gradient linear form
 *  RADIAL draw gradietn circle form
 * UnitType has two types : OBJECT_BOUNDING_BOX / USER_SPACE
 *  OBJECT_BOUNDING_BOX use normalized coordinate, relate by Actor bounding box. bottom-left ~ top-right : (-0.5,-0.5) ~ (0.5, 0.5)
 *  USER_SPACE use coordinate, relate by Actor Size. bottom-left ~ top-right : (ActorSize * -0.5) ~ (ActorSize * 0.5)
 * SpreadType has three types : REFLECT / REPEAT / CLAMP
 *  REFLECT use mirror warping
 *  REPEAT use repeat warping
 *  CLAMP use clamp warping
 *
 * DirectionType has two types : FORWARD / BACKWARD
 *  FORWARD animate value from start to target
 *  BACKWARD animate value from target to start
 * MotionType has two types : LOOP / MIRROR
 *  LOOP animate loopingmode restart
 *  MIRROR animate loopingmode auto_reverse
 * EasingType has four types : LINEAR / IN / OUT / IN_OUT
 *  LINEAR easing animation linear
 *  IN easing in (slow start -> fast finish)
 *  OUT easing out (fast start -> slow finish)
 *  IN_OUT easing in and out (slow start -> slow finish)
 */
class AnimatedGradientVisual : public Visual::Base
{
public:

  /**
   * Animation informations what this visual using
   */
  struct GradientAnimationData
  {
   GradientAnimationData()
    : index( Property::INVALID_INDEX ),
      loop_count( 0 ),
      delay( 0.0f ),
      forward( false ),
      auto_mirror( false )
    {
    }

    Toolkit::TransitionData transition;
    Animation animation;
    Property::Index index;
    int loop_count;   ///< if < 0, loop unlimited. else, loop loop_count times.
    float delay;      ///< delay time. if > 0, wait 'delay' seconds. else, play animation at '-delay' seconds.
    bool forward;     ///< True if AnimationParameter::DirectionType::Type is FORWARD
    bool auto_mirror; ///< True if AnimationParameter::LoopType::Type is MIRROR
  };

  using GradientAnimationDataList =  Dali::OwnerContainer< GradientAnimationData* >;

public:

  /**
   * @brief Create a new animated gradient visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual
   */
  static AnimatedGradientVisualPtr New( VisualFactoryCache& factoryCache, const Property::Map& properties );

private: //from Visual

  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  void DoCreatePropertyMap( Property::Map& map ) const override;

  /**
   * @copydoc Visual::Base::CreateInstancePropertyMap
   */
  void DoCreateInstancePropertyMap( Property::Map& map ) const override;

protected:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  AnimatedGradientVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unrefecence()
   */
  virtual ~AnimatedGradientVisual();

protected: //from Visual

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  void DoSetProperties( const Property::Map& propertyMap ) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  void OnSetTransform() override;

  /**
   * @copydoc Visual::Base::DoSetOnScene
   */
  void DoSetOnScene( Actor& actor ) override;

  /**
   * @copydoc Visual::Base::DoSetOffScene
   */
  void DoSetOffScene( Actor& actor ) override;

private:

  /**
   * @brief Initialize the default value of properies.
   */
  void SetupDefaultValue();

  /**
   * @brief Initialize the rendere with the geometry from the cache, and shader which made by CreateShader()
   */
  void InitializeRenderer();

  /**
   * @brief Make animations with GradientAnimationData
   */
  void SetupAnimation();

  /**
  * @brief Play animations
  */
  void PlayAnimation();

  /**
  * @brief Stop animations
  */
  void StopAnimation();

  /**
   * @brief Clear all previous GradientAnimationData and Setup new GradientAnimationData information which made by animated properties
   *
   * param[in] propertyMap A Property::Map come from DoSetProperties
   */
  void SetupGradientAnimationData( const Property::Map& propertyMap );

  /**
   * @brief Create new shader
   *
   * return A Shader which made by non-animated properties
   */
  Shader CreateShader();

  // Undefined
  AnimatedGradientVisual( const AnimatedGradientVisual& gradientRenderer );

  // Undefined
  AnimatedGradientVisual& operator=( const AnimatedGradientVisual& gradientRenderer );

private:
  GradientAnimationDataList mGradientAnimationDataList;
  Property::Map mValueMap;

  Dali::Toolkit::DevelAnimatedGradientVisual::GradientType::Type mGradientType;
  Dali::Toolkit::DevelAnimatedGradientVisual::UnitType::Type mUnitType;
  Dali::Toolkit::DevelAnimatedGradientVisual::SpreadType::Type mSpreadType;
};

}//namespace Internal

}//namespace Toolkit

}//namespace Dali

#endif
