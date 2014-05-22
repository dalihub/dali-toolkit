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
#include <boost/bind.hpp>

#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-carousel-effect-impl.h>

using namespace Dali;

namespace // unnamed namespace
{

/**
 * Gets a property index. If the property doesn't already exist, then
 * it will create the property.
 * @param[in] handle The handle that owns or will own the property
 * @param[in] name The name for this property
 * @param[in] propertyValue The initial value for this property
 * @return The property index for this property is returned.
 */
Property::Index SafeRegisterProperty( Handle& handle, const std::string& name, Property::Value propertyValue )
{
  Property::Index index = handle.GetPropertyIndex( name );

  if(index == Property::INVALID_INDEX)
  {
    index = handle.RegisterProperty( name, propertyValue );
  }

  return index;
}

/**
 * ScrollCarouselEffectInfo
 *
 * Visibility constraint: switches off the visibility when Actor
 * is outside of bounds, for performance reasons.
 *
 * Rotate constraint: adjusts the angle of the Actors
 * based on their position relative to the edges of the screen.
 * When in the middle portion of the screen Angle does not change.
 * When leaving the edge of the screen screen rotation changes.
 *
 * Position constraint: adjusts the position of the Actors
 * based on their parent page's position relative to the edges of the screen.
 * The position constraint compensates for the rotation which would otherwise
 * move the Actor's edge visually away from the neighboring actor, as they rotate
 * around their default anchor point.
 */
class ScrollCarouselEffectInfo : public Dali::RefObject
{
public:

  ScrollCarouselEffectInfo(const Vector2& angleSwing)
  : mAngleSwing(angleSwing),
    mCanvasMargin( 0.0f, 0.0f ),
    mVisibilityThreshold( 1.0f, 1.0f )
  {
  }

  /**
   * @param[in] current The current visibility of this Actor
   * @param[in] positionProperty The Actor's Position.
   * @param[in] scaleProperty The Actor's Scale.
   * @param[in] sizeProperty The Actor's Size
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] scrollSizeProperty The size of the scroll-view (scrollView SIZE)
   * @return The new visibility of this Actor.
   */
  bool VisibilityConstraint(const bool& current,
                                  const PropertyInput& positionProperty,
                                  const PropertyInput& scaleProperty,
                                  const PropertyInput& sizeProperty,
                                  const PropertyInput& scrollPositionProperty,
                                  const PropertyInput& scrollSizeProperty)
  {
    const Vector2& anchor(AnchorPoint::CENTER.GetVectorXY());
    Vector2 position(positionProperty.GetVector3() + scrollPositionProperty.GetVector3());
    Vector2 scaledSize(sizeProperty.GetVector3() * scaleProperty.GetVector3());

    Vector2 domain(scrollSizeProperty.GetVector3());

    position -= (anchor - mVisibilityThreshold) * scaledSize;
    domain -= (Vector2::ONE - mVisibilityThreshold * 2.0f) * scaledSize;

    return ( position.x >= 0 &&
             position.x <= domain.x &&
             position.y >= 0 &&
             position.y <= domain.y );
  }

  /**
   * @param[in] current The current orientation of this Actor
   * @param[in] positionProperty The Actor's Position.
   * @param[in] scaleProperty The Actor's Scale.
   * @param[in] sizeProperty The Actor's Size
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] scrollSizeProperty The size of the scroll-view (scrollView SIZE)
   * @param[in] activateProperty Activation value (0 - normal, 1.0 - full effect)
   * @return The new orientation of this Actor.
   */
  Quaternion RotationConstraint(const Quaternion& current,
                                const PropertyInput& positionProperty,
                                const PropertyInput& scaleProperty,
                                const PropertyInput& sizeProperty,
                                const PropertyInput& scrollPositionProperty,
                                const PropertyInput& scrollSizeProperty,
                                const PropertyInput& activateProperty)
  {
    const float activate(activateProperty.GetFloat());

    if(activate <= Math::MACHINE_EPSILON_0)
    {
      return current;
    }

    const Vector2& anchor(AnchorPoint::CENTER.GetVectorXY());
    Vector2 position(positionProperty.GetVector3() + scrollPositionProperty.GetVector3());
    Vector2 scaledSize(sizeProperty.GetVector3() * scaleProperty.GetVector3());
    Vector2 domain(scrollSizeProperty.GetVector3());

    position -= (anchor - mCanvasMargin) * scaledSize;
    domain -= (Vector2::ONE - mCanvasMargin * 2.0f) * scaledSize;

    Vector2 angle;

    if( position.y < 0 )
    {
      angle.y = (-position.y / scaledSize.height) * mAngleSwing.y;
    }
    else if( position.y > domain.y )
    {
      angle.y = ((domain.y - position.y) / scaledSize.height) * mAngleSwing.y;
    }

    angle *= activate;

    return Quaternion(-angle.x, Vector3::YAXIS) *
           Quaternion(angle.y, Vector3::XAXIS) *
           current;
  }

  /**
   * @param[in] current The current position of this Actor
   * @param[in] scaleProperty The Actor's Scale.
   * @param[in] sizeProperty The Actor's Size
   * @param[in] scrollPositionProperty The scroll-view's position property (SCROLL_POSITION_PROPERTY_NAME)
   * @param[in] scrollSizeProperty The size of the scroll-view (scrollView SIZE)
   * @param[in] activateProperty Activation value (0 - normal, 1.0 - full effect)
   * @return The new position of this Actor.
   */
  Vector3 PositionConstraint(const Vector3& current,
                             const PropertyInput& scaleProperty,
                             const PropertyInput& sizeProperty,
                             const PropertyInput& scrollPositionProperty,
                             const PropertyInput& scrollSizeProperty,
                             const PropertyInput& activateProperty)
  {
    const float activate(activateProperty.GetFloat());
    Vector3 position(current + scrollPositionProperty.GetVector3());

    if(activate <= Math::MACHINE_EPSILON_0)
    {
      return position;
    }

    const Vector2& anchor(AnchorPoint::CENTER.GetVectorXY());
    Vector2 scaledSize(sizeProperty.GetVector3() * scaleProperty.GetVector3());
    Vector2 domain(scrollSizeProperty.GetVector3());

    position.GetVectorXY() -= (anchor - mCanvasMargin) * scaledSize;
    domain -= (Vector2::ONE - mCanvasMargin * 2.0f) * scaledSize;

    Vector2 angle;

    if(position.y < 0)
    {
      angle.y = (-position.y / scaledSize.height) * mAngleSwing.y * activate;
      position.y += (1.0f - cosf(angle.y)) * scaledSize.height * 0.5f;
      position.z -= sinf(angle.y) * scaledSize.height * 0.5f;
    }
    else if(position.y > domain.y)
    {
      angle.y = ((domain.y - position.y) / scaledSize.height) * mAngleSwing.y * activate;
      position.y -= (1.0f - cosf(angle.y)) * scaledSize.height * 0.5f;
      position.z -= sinf(-angle.y) * scaledSize.height * 0.5f;
    }

    position.GetVectorXY() += (anchor - mCanvasMargin) * scaledSize;

    return position;
  }

  Vector2 mAngleSwing;                                    ///< Maximum amount in X and Y axes to rotate.
  Vector2 mCanvasMargin;                                  ///< Margin around the canvas for when to start rotating
  Vector2 mVisibilityThreshold;                           ///< Threshold for when to to switch off visibility of Actor (for performance)
};

typedef IntrusivePtr<ScrollCarouselEffectInfo> ScrollCarouselEffectInfoPtr;

/**
 * Helper: Applies the 3D scroll carousel constraints to the child actor
 *
 * @param[in] scrollView The ScrollView containing the pages.
 * @param[in] child The child to be affected with the 3D Effect.
 * @param[in] info The effect info for the constraints
 */
void ApplyScrollCarouselConstraints(Toolkit::ScrollView scrollView,
                                    Actor child,
                                    ScrollCarouselEffectInfoPtr info)
{
  // Apply constraints to this actor //
  Constraint constraint;

  constraint = Constraint::New<bool>( Actor::VISIBLE,
                                      LocalSource( Actor::POSITION ),
                                      LocalSource( Actor::SCALE ),
                                      LocalSource( Actor::SIZE ),
                                      Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
                                      Source(scrollView, Actor::SIZE ),
                                      Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewCarouselEffect::EFFECT_ACTIVATE ) ),
                                      boost::bind( &ScrollCarouselEffectInfo::VisibilityConstraint, info, _1, _2, _3, _4, _5, _6) );
  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  constraint = Constraint::New<Quaternion>( Actor::ROTATION,
                                            LocalSource( Actor::POSITION ),
                                            LocalSource( Actor::SCALE ),
                                            LocalSource( Actor::SIZE ),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
                                            Source(scrollView, Actor::SIZE ),
                                            Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewCarouselEffect::EFFECT_ACTIVATE ) ),
                                            boost::bind( &ScrollCarouselEffectInfo::RotationConstraint, info, _1, _2, _3, _4, _5, _6, _7) );
  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );

  constraint = Constraint::New<Vector3>( Actor::POSITION,
                                         LocalSource( Actor::SCALE ),
                                         LocalSource( Actor::SIZE ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollView::SCROLL_POSITION_PROPERTY_NAME ) ),
                                         Source(scrollView, Actor::SIZE ),
                                         Source(scrollView, scrollView.GetPropertyIndex( Toolkit::ScrollViewCarouselEffect::EFFECT_ACTIVATE ) ),
                                         boost::bind( &ScrollCarouselEffectInfo::PositionConstraint, info, _1, _2, _3, _4, _5, _6) );

  constraint.SetRemoveAction( Constraint::Discard );
  child.ApplyConstraint( constraint );
}

} // unnamed namespace

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

ScrollViewCarouselEffect::ScrollViewCarouselEffect()
: mPropertyActivate(Property::INVALID_INDEX)
{
}

ScrollViewCarouselEffect::~ScrollViewCarouselEffect()
{
}

void ScrollViewCarouselEffect::ApplyToActor(Actor child, const Vector2& angleSwing)
{
  ScrollCarouselEffectInfoPtr info(new ScrollCarouselEffectInfo(angleSwing));

  ApplyScrollCarouselConstraints( GetScrollView(), child, info );
}

void ScrollViewCarouselEffect::OnAttach(Toolkit::ScrollView& scrollView)
{
  if(mPropertyActivate == Property::INVALID_INDEX)
  {
    mPropertyActivate = SafeRegisterProperty( scrollView, Toolkit::ScrollViewCarouselEffect::EFFECT_ACTIVATE, 1.0f );
  }
}

void ScrollViewCarouselEffect::OnDetach(Toolkit::ScrollView& scrollView)
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
