#ifndef DALI_TOOLKIT_INTERNAL_ARC_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_ARC_VISUAL_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/intrusive-ptr.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/devel-api/visuals/arc-visual-properties-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class ArcVisual;
typedef IntrusivePtr< ArcVisual > ArcVisualPtr;

/**
 * The visual which renders an arc to the control's quad
 *
 * The following properties are required for create an ArcVisual
 *
 * | %Property Name  | Type        |
 * |-----------------|-------------|
 * | THICKNESS       | FLOAT       |
 * | START_ANGLE     | FLOAT       |
 * | SWEEP_ANGLE     | FLOAT       |
 * | CAP             | INTEGER     |
 */
class ArcVisual: public Visual::Base
{
public:

  /**
   * @brief Create a new arc visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static ArcVisualPtr New( VisualFactoryCache& factoryCache, const Property::Map& properties );

public:  // from Visual

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
  ArcVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~ArcVisual();

  /**
   * @copydoc Visual::Base::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  void DoSetProperties( const Property::Map& propertyMap ) override;

  /**
   * @copydoc Visual::Base::DoSetOnScene
   */
  void DoSetOnScene( Actor& actor ) override;

  /**
   * @copydoc Visual::Base::DoSetOffScene
   */
  void DoSetOffScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  void OnSetTransform() override;

  /**
   * @copydoc Visual::Base::OnDoAction
   */
  void OnDoAction( const Property::Index actionId, const Property::Value& attributes ) override;

private:

  // Undefined
  ArcVisual( const ArcVisual& arcVisual ) = delete;

  // Undefined
  ArcVisual& operator=( const ArcVisual& arcVisual ) = delete;

private:

  float mThickness;                    ///< The thickness of the arc.
  float mRadius;                       ///< The radius of the arc.
  float mStartAngle;                   ///< The start angle of the arc.
  float mSweepAngle;                   ///< The sweep angle of the arc.
  Property::Index mRadiusIndex;        ///< The index of the radius property.
  Property::Index mThicknessIndex;     ///< The index of the thickness property.
  Property::Index mStartAngleIndex;    ///< The index of the start angle property.
  Property::Index mSweepAngleIndex;    ///< The index of the sweep angle property.
  DevelArcVisual::Cap::Type mCapType;  ///< The cap type.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_ARC_VISUAL_H */
